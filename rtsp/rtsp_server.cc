/*
 * rtsp_server.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "rtsp_server.h"

#include <cstdlib>
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
      video_queue_(std::make_shared<VideoQueue>()),
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
  // session->AddSource(xop::channel_0, xop::H264Source::CreateNew());
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

  media_session_ = session;
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

void RtspServer::RequestVideoSink(int32_t stream_id, CodecId codec_id) {
  auto msg =
      std::make_shared<Message>(kWhatRequestVideoSink, shared_from_this());

  msg->setInt32("stream_id", stream_id);
  msg->setInt32("codec_format", static_cast<int32_t>(codec_id));
  msg->post();
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

  auto notify = notify_->dup();
  notify->setInt32("what", kWhatClientConnectedNotify);
  notify->post();
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

  auto notify = notify_->dup();
  notify->setInt32("what", kWhatClientDisconnectedNotify);
  notify->post();
}

void RtspServer::OnRequestVideoSink(const std::shared_ptr<Message>& msg) {
  int32_t stream_id;
  CHECK(msg->findInt32("stream_id", &stream_id));
  int32_t codec_id;
  CHECK(msg->findInt32("codec_format", &codec_id));

  if (media_session_ == nullptr) {
    return;
  }
  CodecId codec = static_cast<CodecId>(codec_id);
  switch (codec) {
    case CodecId::AV_CODEC_ID_H264: {
      media_session_->AddSource(xop::channel_0, xop::H264Source::CreateNew());
      break;
    }
    case CodecId::AV_CODEC_ID_VP8: {
      media_session_->AddSource(xop::channel_0, xop::VP8Source::CreateNew());
      break;
    }

    default: {
      break;
    }
  }

  LOG(LS_INFO) << "add video sink, stream_id: " << stream_id
               << ", codec_id: " << codec_id;

  auto notify = notify_->dup();
  notify->setInt32("what", kWhatVideoSinkAdded);
  notify->setInt32("stream_id", stream_id);
  notify->setObject("encoded_video_sink", video_queue_);
  notify->post();

  auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
  m->post();
}

void RtspServer::OnPullAudioSource() {
  std::lock_guard<std::mutex> l(mutex_);
  if (started_) {
    auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
    // TODO calculate delay
    m->post(50 * 1000);
  }
}

void RtspServer::OnPullVideoSource() {
  // get one from VideoQueue.queue
  if (!video_queue_->queue().empty()) {
    EncodedImage image = video_queue_->queue().front();
    video_queue_->queue().pop();
    xop::AVFrame frame = {0};
    frame.type = 0;
    frame.size = image.Size();
    // int64_t pts = image.Timestamp();
    // CHECK(buffer->meta()->findInt64("timeUs", &pts));
    frame.timestamp = image.Timestamp() / 1000 * 90;

    if (image.frame_type_ == VideoFrameType::kVideoFrameKey) {
      LOG(LS_INFO) << "OnPullVideoSource, queue.size:"
                   << video_queue_->queue().size()
                   << ", image size: " << image.Size()
                   << ", timestamp_us: " << image.Timestamp()
                   << ", frame_type:" << image.frame_type_;
    }

    frame.buffer.reset(new uint8_t[frame.size]);
    memcpy(frame.buffer.get(), image.Data(), frame.size);

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

    case kWhatRequestVideoSink: {
      OnRequestVideoSink(msg);
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
