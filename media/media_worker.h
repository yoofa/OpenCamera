/*
 * media_worker.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_WORKER_H
#define MEDIA_WORKER_H

#include "api/audio/audio_device.h"
#include "api/audio/audio_frame.h"
#include "api/audio/audio_sink_interface.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "api/video_codecs/video_encoder_config.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "base/task_util/task.h"
#include "base/task_util/task_runner_factory.h"

namespace avp {

class MediaWorker {
  using VideoSource =
      std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>;
  using EncodedVideoSink = std::shared_ptr<VideoSinkInterface<EncodedImage>>;

 public:
  explicit MediaWorker(base::TaskRunnerFactory* task_runner_factory,
                       AudioEncoderFactory* audio_encoder_factory,
                       VideoEncoderFactory* video_encoder_factory,
                       AudioDevice* audio_device)

      : task_runner_factory_(task_runner_factory),
        audio_encoder_factory_(audio_encoder_factory),
        video_encoder_factory_(video_encoder_factory),
        audio_device_(audio_device) {}
  virtual ~MediaWorker() = default;

  virtual void AddVideoSource(VideoSource& video_source,
                              int32_t stream_id,
                              VideoEncoderConfig& encoder_config) = 0;
  virtual void RemoveVideoSource(VideoSource& video_source,
                                 int32_t stream_id) = 0;

  virtual void AddEncodedVideoSink(EncodedVideoSink& encoded_image_sink,
                                   int32_t id) {}
  virtual void RemoveEncodedVideoSink(EncodedVideoSink& encoded_image_sink,
                                      int32_t stream_id) {}

  virtual void RequestKeyFrame() {}

  virtual void AddEncodedAudioSink(
      std::shared_ptr<AudioSinkInterface<MediaPacket>>& audio_sink,
      int32_t stream_id,
      CodecId codec_id) = 0;
  virtual void RemoveEncodedAudioSink(
      std::shared_ptr<AudioSinkInterface<MediaPacket>>& audio_sink,
      CodecId codec_id) = 0;

  virtual void AddAudioStreamReceiver() = 0;
  virtual void RemoveAudioStreamReceiver() = 0;

 protected:
  base::TaskRunnerFactory* task_runner_factory() const {
    return task_runner_factory_;
  }

  VideoEncoderFactory* video_encoder_factory() const {
    return video_encoder_factory_;
  }

  AudioEncoderFactory* audio_encoder_factory() const {
    return audio_encoder_factory_;
  }

  AudioDevice* audio_device() const { return audio_device_; }

 private:
  base::TaskRunnerFactory* task_runner_factory_;
  AudioEncoderFactory* audio_encoder_factory_;
  VideoEncoderFactory* video_encoder_factory_;
  AudioDevice* audio_device_;
};

}  // namespace avp

#endif /* !MEDIA_WORKER_H */
