/*
 * h264.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AVE_VIDEO_CODECS_H264_H
#define AVE_VIDEO_CODECS_H264_H

#include <memory>

#include "api/video_codecs/video_encoder.h"
#include "common/video_codec_property.h"

namespace ave {

class H264Encoder : public VideoEncoder {
 public:
  H264Encoder() = default;
  virtual ~H264Encoder() = default;

  static std::unique_ptr<H264Encoder> Create();
};

}  // namespace ave

#endif /* !AVE_VIDEO_CODECS_H264_H */
