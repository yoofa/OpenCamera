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
  status_t Init();
  status_t Start();
  status_t Stop();

  status_t AddMediaSource(std::shared_ptr<MediaSource> mediaSource);

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
  void OnClientConnected(const std::shared_ptr<Message>& msg);
  void OnClientDisconnected(const std::shared_ptr<Message>& msg);
  void OnAddMediaSource(const std::shared_ptr<Message>& msg);
  void OnPullAudioSource();
  void OnPullVideoSource();

  void OnStart(const std::shared_ptr<Message>& message);
  void OnStop(const std::shared_ptr<Message>& message);

  // handler
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  std::shared_ptr<Message> notify_;
  std::shared_ptr<Looper> looper_;
  std::shared_ptr<xop::EventLoop> event_loop_;
  std::shared_ptr<xop::RtspServer> server_;
  xop::MediaSessionId session_id_;

  bool has_audio_;
  bool has_video_;
  bool started_;
  std::shared_ptr<MediaSource> audio_source_;
  std::shared_ptr<MediaSource> video_source_;
  std::mutex mutex_;
};
}  // namespace avp

#endif /* !RTSPSERVER_H */
