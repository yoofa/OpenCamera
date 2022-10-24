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
    : mNotify(std::move(notify)),
      mLooper(new Looper()),
      mEventLoop(new xop::EventLoop()),
      mServer(xop::RtspServer::Create(mEventLoop.get())),
      mSessionId(-1),
      mHasAudio(false),
      mHasVideo(false),
      mStarted(false) {
  mLooper->setName("RtspServer");
}

RtspServer::~RtspServer() {
  mLooper->unregisterHandler(id());
  mLooper->stop();
}

void RtspServer::init() {
  std::lock_guard<std::mutex> l(mLock);
  mLooper->start();
  mLooper->registerHandler(shared_from_this());

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

  mSessionId = mServer->AddSession(session);
}

void RtspServer::start() {
  auto msg = std::make_shared<Message>(kWhatStart, shared_from_this());
  msg->post();
}

void RtspServer::stop() {
  auto msg = std::make_shared<Message>(kWhatStop, shared_from_this());
  msg->post();
}

status_t RtspServer::addMediaSource(std::shared_ptr<MediaSource> mediaSource) {
  auto msg = std::make_shared<Message>(kWhatAddMediaSource, shared_from_this());
  msg->setObject("mediaSource", std::move(mediaSource));
  msg->post();
  return OK;
}

void RtspServer::onClientConnected(const std::shared_ptr<Message>& msg) {
  int32_t sessionId = 0;
  std::string ip{};
  int32_t port = 0;
  DCHECK(msg->findInt32("sessionId", &sessionId));
  DCHECK(msg->findString("ip", ip));
  DCHECK(msg->findInt32("port", &port));
  LOG(LS_INFO) << "client connect, sessionId: " << sessionId << ", ip: " << ip
               << ", port: " << port;
}

void RtspServer::onClientDisconnected(const std::shared_ptr<Message>& msg) {
  int32_t sessionId = 0;
  std::string ip{};
  int32_t port = 0;
  DCHECK(msg->findInt32("sessionId", &sessionId));
  DCHECK(msg->findString("ip", ip));
  DCHECK(msg->findInt32("port", &port));
  LOG(LS_INFO) << "client disconnect, sessionId: " << sessionId
               << ", ip: " << ip << ", port: " << port;
}

void RtspServer::onAddMediaSource(const std::shared_ptr<Message>& msg) {
  std::shared_ptr<MessageObject> obj;
  DCHECK(msg->findObject("mediaSource", obj));
  std::shared_ptr<MediaSource> source =
      std::dynamic_pointer_cast<MediaSource>(obj);

  auto meta = source->getFormat();
  auto format = std::make_shared<Message>();
  if (convertMetaDataToMessage(meta, format) != OK) {
    return;
  }

  std::string mime;
  DCHECK(format->findString("mime", mime));
  bool isAudio = !strncasecmp("audio/", mime.c_str(), 6);
  if (isAudio && mHasAudio) {
    LOG(LS_WARNING)
        << "add media source failed, only support on audio stream, return";
    return;
  }

  if (!isAudio && mHasVideo) {
    LOG(LS_WARNING)
        << "add media source failed, only support on video stream, return";
    return;
  }
  if (isAudio) {
    mAudioSource = std::move(source);
    std::lock_guard<std::mutex> l(mLock);
    if (mStarted) {
      auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
      m->post();
    }
  } else {
    mVideoSource = std::move(source);
    std::lock_guard<std::mutex> l(mLock);
    if (mStarted) {
      auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
      m->post();
    }
  }
}

void RtspServer::onPullAudioSource() {
  if (mAudioSource.get() == nullptr) {
    return;
  }

  std::shared_ptr<Buffer> buffer;
  status_t ret = mAudioSource->read(buffer);
  if (ret == OK) {
    xop::AVFrame frame = {0};
    frame.type = xop::AUDIO_FRAME;
    frame.size = buffer->size();
    int64_t pts = 0;
    DCHECK(buffer->meta()->findInt64("timeUs", &pts));
    frame.timestamp = pts;

    frame.buffer.reset(new uint8_t[frame.size]);
    memcpy(frame.buffer.get(), buffer->data(), frame.size);

    mServer->PushFrame(mSessionId, xop::channel_1, frame);
  }

  std::lock_guard<std::mutex> l(mLock);
  if (mStarted) {
    auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
    // TODO calculate delay
    m->post(50 * 1000);
  }
}

void RtspServer::onPullVideoSource() {
  if (mVideoSource.get() == nullptr) {
    return;
  }

  std::shared_ptr<Buffer> buffer;
  status_t ret = mVideoSource->read(buffer);
  if (ret == OK) {
    xop::AVFrame frame = {0};
    frame.type = 0;
    frame.size = buffer->size();
    int64_t pts = 0;
    DCHECK(buffer->meta()->findInt64("timeUs", &pts));
    frame.timestamp = pts / 1000 * 90;

    frame.buffer.reset(new uint8_t[frame.size]);
    memcpy(frame.buffer.get(), buffer->data(), frame.size);

    mServer->PushFrame(mSessionId, xop::channel_0, frame);
  }

  std::lock_guard<std::mutex> l(mLock);
  if (mStarted) {
    auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
    // TODO calculate delay
    m->post(40 * 1000);
  }
}
void RtspServer::onStart(const std::shared_ptr<Message>& msg) {
  mServer->Start("0.0.0.0", 8554);
  auto source = mAudioSource;
  if (source.get() != nullptr) {
    auto m = std::make_shared<Message>(kWhatPullAudio, shared_from_this());
    m->post();
  }
  source = mVideoSource;

  if (source.get() != nullptr) {
    auto m = std::make_shared<Message>(kWhatPullVideo, shared_from_this());
    m->post();
  }

  mStarted = true;
}
void RtspServer::onStop(const std::shared_ptr<Message>& msg) {
  mServer->Stop();
  mStarted = false;
}

void RtspServer::onMessageReceived(const std::shared_ptr<Message>& msg) {
  switch (msg->what()) {
    case kWhatStart: {
      onStart(msg);
      break;
    }

    case kWhatStop: {
      onStop(msg);
      break;
    }

    case kWhatClientConnected: {
      onClientConnected(msg);
      break;
    }

    case kWhatClientDisonnected: {
      onClientDisconnected(msg);
      break;
    }

    case kWhatAddMediaSource: {
      onAddMediaSource(msg);
      break;
    }
    case kWhatPullAudio: {
      onPullAudioSource();
      break;
    }
    case kWhatPullVideo: {
      onPullVideoSource();
      break;
    }
  }
}

}  // namespace avp
