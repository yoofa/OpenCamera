/*
 * video_encoder_factory.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_ENCODER_FACTORY_H
#define VIDEO_ENCODER_FACTORY_H

#include <memory>

#include "api/video_codecs/video_encoder.h"

namespace ave {

class VideoEncoderFactory {
 public:
  virtual ~VideoEncoderFactory() = default;

  virtual std::unique_ptr<VideoEncoder> CreateVideoEncoder() = 0;
};

}  // namespace ave
#endif /* !VIDEO_ENCODER_FACTORY_H */
