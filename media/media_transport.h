/*
 * media_transport.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_TRANSPORT_H
#define MEDIA_TRANSPORT_H

#include <memory>
#include <ostream>

#include "api/audio/audio_frame.h"
#include "api/audio/audio_sink_interface.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/thread_annotation.h"
#include "media/audio/audio_stream_sender.h"
#include "media/video/video_stream_sender.h"

namespace ave {
class MediaTransport {
  using EncodedVideoSink = std::shared_ptr<VideoSinkInterface<EncodedImage>>;
  using EncodedAudioBuffer = MediaPacket;
  using EncodedAudioSink =
      std::shared_ptr<AudioSinkInterface<EncodedAudioBuffer>>;

 public:
  MediaTransport(base::TaskRunnerFactory* task_runner_factory);
  ~MediaTransport();

  VideoStreamSender* GetVideoStreamSender(int32_t id)
      //    REQUIRES(transport_runner_)
      ;
  void RemoveVideoStreamSender(int32_t id);

  void AddVideoSink(const EncodedVideoSink& sink, int32_t id);
  void RemoveVideoSink(const EncodedVideoSink& sink);
  bool frame_wanted(int32_t stream_id) const;

  AudioStreamSender* GetAudioStreamSender(int32_t id, CodecId codec_id);
  void RemoveAudioStreamSender(CodecId codec_id);

  void AddAudioSenderSink(const EncodedAudioSink sink,
                          int32_t stream_id,
                          CodecId codec_id);
  void RemoveAudioSenderSink(const EncodedAudioSink& sink);

 private:
  struct VideoStreamSenderInfo {
    std::unique_ptr<VideoStreamSender> video_stream_sender;
    int32_t id;
  };

  struct AudioStreamSenderInfo {
    std::unique_ptr<AudioStreamSender> audio_stream_sender;
    int32_t stream_id;
    CodecId codec_id;
  };

  base::TaskRunner transport_runner_;
  std::vector<VideoStreamSenderInfo> video_stream_senders_;
  std::vector<AudioStreamSenderInfo> audio_stream_senders_;
};

}  // namespace ave

#endif /* !MEDIA_TRANSPORT_H */
