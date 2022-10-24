/*
 * rtsp_server.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <memory>

#include "common/handler.h"
#include "common/looper.h"
#include "common/media_source.h"
#include "common/message.h"
#include "third_party/rtsp_server/src/src/xop/RtspServer.h"

namespace avp {
class RtspServer : public Handler {
 public:
  RtspServer(std::shared_ptr<Message> notify);
  virtual ~RtspServer();
  void init();
  void start();
  void stop();

  status_t addMediaSource(std::shared_ptr<MediaSource> mediaSource);

  enum {

    kWhatStart = 'star',
    kWhatStop = 'stop',

    kWhatClientConnected = 'cnet',
    kWhatClientDisonnected = 'dcnt',

    kWhatAddMediaSource = 'adms',
    kWhatPullAudio = 'pula',
    kWhatPullVideo = 'pulv',
  };

 private:
  void onClientConnected(const std::shared_ptr<Message>& msg);
  void onClientDisconnected(const std::shared_ptr<Message>& msg);
  void onAddMediaSource(const std::shared_ptr<Message>& msg);
  void onPullAudioSource();
  void onPullVideoSource();

  void onStart(const std::shared_ptr<Message>& message);
  void onStop(const std::shared_ptr<Message>& message);
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  std::shared_ptr<Message> mNotify;
  std::shared_ptr<Looper> mLooper;
  std::shared_ptr<xop::EventLoop> mEventLoop;
  std::shared_ptr<xop::RtspServer> mServer;
  xop::MediaSessionId mSessionId;

  bool mHasAudio;
  bool mHasVideo;
  bool mStarted;
  std::shared_ptr<MediaSource> mAudioSource;
  std::shared_ptr<MediaSource> mVideoSource;
  std::mutex mLock;
};
}  // namespace avp

#endif /* !RTSPSERVER_H */
