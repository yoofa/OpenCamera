/*
 * rtsp_server.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <memory>

#include "api/audio/audio_sink_interface.h"
#include "api/video/encoded_image.h"
#include "api/video/video_sink_interface.h"
#include "base/buffer.h"
#include "base/logging.h"
#include "common/codec_id.h"
#include "common/handler.h"
#include "common/looper.h"
#include "common/media_source.h"
#include "common/message.h"
#include "third_party/rtsp_server/src/src/xop/RtspServer.h"

namespace avp {
class RtspServer : public Handler {
 public:
  class VideoQueue : public VideoSinkInterface<EncodedImage>,
                     public MessageObject {
   public:
    void OnFrame(const EncodedImage& frame) override { queue_.push(frame); }
    std::queue<EncodedImage>& queue() { return queue_; }

   private:
    std::queue<EncodedImage> queue_;
  };

  class AudioQueue : public AudioSinkInterface<std::shared_ptr<Buffer8>>,
                     public MessageObject {
   public:
    void SetSampleRate(int sample_rate) { set_sample_rate(sample_rate); }
    void SetChannelCount(int channel_count) {
      set_channel_count(channel_count);
    }

    void OnFrame(const std::shared_ptr<Buffer8> frame) override {
      queue_.push(frame);
    }
    std::queue<std::shared_ptr<Buffer8>>& queue() { return queue_; }

   private:
    std::queue<std::shared_ptr<Buffer8>> queue_;
  };

  RtspServer(std::shared_ptr<Message> notify);
  virtual ~RtspServer();
  status_t Init();
  status_t Start();
  status_t Stop();

  void RequestVideoSink(int32_t stream_id, CodecId codec_id);
  void RequestAudioSink(int32_t stream_id,
                        CodecId codec_id,
                        int sample_rate,
                        int channels);

  enum {

    kWhatStart = 'star',
    kWhatStop = 'stop',

    kWhatClientConnected = 'cnet',
    kWhatClientDisonnected = 'dcnt',

    kWhatRequestAudioSink = 'raud',
    kWhatRequestVideoSink = 'rvid',

    kWhatPullAudio = 'pula',
    kWhatPullVideo = 'pulv',

    // notify
    kWhatClientConnectedNotify = 'ccnt',
    kWhatClientDisconnectedNotify = 'dcnt',
    kWhatAudioSinkAdded = 'asin',
    kWhatVideoSinkAdded = 'vsin',
  };

 private:
  void OnClientConnected(const std::shared_ptr<Message>& msg);
  void OnClientDisconnected(const std::shared_ptr<Message>& msg);
  void OnAddMediaSource(const std::shared_ptr<Message>& msg);

  void OnRequestAudioSink(const std::shared_ptr<Message>& msg);
  void OnRequestVideoSink(const std::shared_ptr<Message>& msg);
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
  xop::MediaSession* media_session_;

  std::shared_ptr<AudioQueue> audio_queue_;
  std::shared_ptr<VideoQueue> video_queue_;

  bool has_audio_;
  bool has_video_;
  bool started_;

  std::mutex mutex_;
};
}  // namespace avp

#endif /* !RTSPSERVER_H */
