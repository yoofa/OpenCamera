/*
 * rtsp_server.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "rtsp_server.h"

#include <memory>

#include "base/checks.h"
#include "base/logging.h"
#include "common/buffer.h"
#include "common/message.h"
#include "common/utils.h"
#include "third_party/rtsp_server/src/src/net/EventLoop.h"

namespace avp {

RtspServer::RtspServer(std::shared_ptr<Message> notify)
    : notify_(std::move(notify)),
      looper_(new Looper()),
      event_loop_(new xop::EventLoop()),
      server_(xop::RtspServer::Create(event_loop_.get())),
      session_id_(-1),
      has_audio_(false),
      has_video_(false),
      started_(false) {
  looper_->setName("RtspServer");
}

RtspServer::~RtspServer() {
  looper_->unregisterHandler(id());
  looper_->stop();
}

status_t RtspServer::Init() {
  std::lock_guard<std::mutex> l(mutex_);
  looper_->start();
  looper_->registerHandler(shared_from_this());

  xop::MediaSession* session = xop::MediaSession::CreateNew("live");
  session->AddSource(xop::channel_0, xop::H264Source::CreateNew());
  session->AddSource(xop::channel_1, xop::AACSource::CreateNew(44100, 2));

  session->AddNotifyConnectedCallback(
      [shared_this = shared_from_this()](xop::MediaSessionId sessionId,
                                         std::string peerIp,
                                         uint16_t peerPort) {
        auto msg = std::make_shared<Message>(kWhatClientConnected, shared_this);
        msg->setInt32("sessionId", sessionId);
        msg->setString("ip", peerIp);
        msg->setInt32("port", peerPort);
        msg->post();
      });

  session->AddNotifyDisconnectedCallback([shared_this = shared_from_this()](
                                             xop::MediaSessionId sessionId,
                                             std::string peerIp,
                                             uint16_t peerPort) {
    auto msg = std::make_shared<Message>(kWhatClientDisonnected, shared_this);
    msg->setInt32("sessionId", sessionId);
    msg->setString("ip", peerIp);
    msg->setInt32("port", peerPort);
    msg->post();
  });

  session_id_ = server_->AddSession(session);
  return OK;
}

status_t RtspServer::Start() {
  auto msg = std::make_shared<Message>(kWhatStart, shared_from_this());
  msg->post();
  return OK;
}

status_t RtspServer::Stop() {
  auto msg = std::make_shared<Message>(kWhatStop, shared_from_this());
  msg->post();
  return OK;
}

status_t RtspServer::AddMediaSource(std::shared_ptr<MediaSource> mediaSource) {
  auto msg = std::make_shared<Message>(kWhatAddMediaSource, shared_from_this());
  msg->setObject("mediaSource", std::move(mediaSource));
  msg->post();
  return OK;
}

void RtspServer::OnClientConnected(const std::shared_ptr<Message>& msg) {
  int32_t sessionId = 0;
  std::string ip{};
  int32_t port = 0;
  CHECK(msg->findInt32("sessionId", &sessionId));
  CHECK(msg->findString("ip", ip));
  CHECK(msg->findInt32("port", &port));
  LOG(LS_INFO) << "client connect, sessionId: " << sessionId << ", ip: " << ip
               << ", port: " << port;
}

void RtspServer::OnClientDisconnected(const std::shared_ptr<Message>& msg) {
  int32_t sessionId = 0;
  std::string ip{};
  int32_t port = 0;
  CHECK(msg->findInt32("sessionId", &sessionId));
  CHECK(msg->findString("ip", ip));
  CHECK(msg->findInt32("port", &port));
  LOG(LS_INFO) << "client disconnect, sessionId: " << sessionId
               << ", ip: " << ip << ", port: " << port;
}

void RtspServer::OnAddMediaSource(const std::shared_ptr<Message>& msg) {
  std::shared_ptr<MessageObject> obj;
  CHECK(msg->findObject("mediaSource", obj));
  std::shared_ptr<MediaSource> source =
      std::dynamic_pointer_cast<MediaSource>(obj);

  auto meta = source->getFormat();
  auto format = std::make_shared<Message>();
  if (convertMetaDataToMessage(meta, format) != OK) {
    return;
  }

  std::string mime;
  CHECK(format->findString("mime", mime));
  bool isAudio = !strncasecmp("audio/", mime.c_str(), 6);
  if (isAudio && has_audio_) {
    LOG(LS_WARNING)
        << "add media source failed, only support on audio stream, return";
    return;
  }

  if (!isAudio && has_video_) {
    LOG(LS_WARNING)
        << "add media source failed, only support on video stream, return";
    return;
  }
  if (isAudio) {
    audio_source_ = std::move(source);
    std::lock_guard<std::mutex> l(mutex_);
    if (started_) {
      auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
      m->post();
    }
  } else {
    video_source_ = std::move(source);
    std::lock_guard<std::mutex> l(mutex_);
    if (started_) {
      auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
      m->post();
    }
  }
}

void RtspServer::OnPullAudioSource() {
  if (audio_source_.get() == nullptr) {
    return;
  }

  std::shared_ptr<Buffer> buffer;
  status_t ret = audio_source_->read(buffer);
  if (ret == OK) {
    xop::AVFrame frame = {0};
    frame.type = xop::AUDIO_FRAME;
    frame.size = buffer->size();
    int64_t pts = 0;
    CHECK(buffer->meta()->findInt64("timeUs", &pts));
    frame.timestamp = pts;

    frame.buffer.reset(new uint8_t[frame.size]);
    memcpy(frame.buffer.get(), buffer->data(), frame.size);

    server_->PushFrame(session_id_, xop::channel_1, frame);
  }

  std::lock_guard<std::mutex> l(mutex_);
  if (started_) {
    auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
    // TODO calculate delay
    m->post(50 * 1000);
  }
}

void RtspServer::OnPullVideoSource() {
  if (video_source_.get() == nullptr) {
    return;
  }

  std::shared_ptr<Buffer> buffer;
  status_t ret = video_source_->read(buffer);
  if (ret == OK) {
    xop::AVFrame frame = {0};
    frame.type = 0;
    frame.size = buffer->size();
    int64_t pts = 0;
    CHECK(buffer->meta()->findInt64("timeUs", &pts));
    frame.timestamp = pts / 1000 * 90;

    frame.buffer.reset(new uint8_t[frame.size]);
    memcpy(frame.buffer.get(), buffer->data(), frame.size);

    server_->PushFrame(session_id_, xop::channel_0, frame);
  }

  std::lock_guard<std::mutex> l(mutex_);
  if (started_) {
    auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
    // TODO calculate delay
    m->post(40 * 1000);
  }
}
void RtspServer::OnStart(const std::shared_ptr<Message>& msg) {
  server_->Start("0.0.0.0", 8554);
  auto source = audio_source_;
  if (source.get() != nullptr) {
    auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
    m->post();
  }
  source = video_source_;

  if (source.get() != nullptr) {
    auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
    m->post();
  }
  LOG(LS_INFO) << "rtsp server run at 127.0.0.1:8554/live";

  started_ = true;
}
void RtspServer::OnStop(const std::shared_ptr<Message>& msg) {
  server_->Stop();
  started_ = false;
}

void RtspServer::onMessageReceived(const std::shared_ptr<Message>& msg) {
  switch (msg->what()) {
    case kWhatStart: {
      OnStart(msg);
      break;
    }

    case kWhatStop: {
      OnStop(msg);
      break;
    }

    case kWhatClientConnected: {
      OnClientConnected(msg);
      break;
    }

    case kWhatClientDisonnected: {
      OnClientDisconnected(msg);
      break;
    }

    case kWhatAddMediaSource: {
      OnAddMediaSource(msg);
      break;
    }
    case kWhatPullAudio: {
      OnPullAudioSource();
      break;
    }
    case kWhatPullVideo: {
      OnPullVideoSource();
      break;
    }
  }
}

}  // namespace avp
