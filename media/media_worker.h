/*
 * media_worker.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_WORKER_H
#define MEDIA_WORKER_H

#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "api/video_codecs/video_encoder_config.h"
#include "api/video_codecs/video_encoder_factory.h"

namespace avp {

class MediaWorker {
  using VideoSource =
      std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>;
  using EncodedVideoSink = std::shared_ptr<VideoSinkInterface<EncodedImage>>;

 public:
  explicit MediaWorker(VideoEncoderFactory* video_encoder_factory)
      : video_encoder_factory_(video_encoder_factory) {}
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

  VideoEncoderFactory* video_encoder_factory() const {
    return video_encoder_factory_;
  }

 private:
  VideoEncoderFactory* video_encoder_factory_;
};

}  // namespace avp

#endif /* !MEDIA_WORKER_H */
