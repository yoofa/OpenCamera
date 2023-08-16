/*
 * h264.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "h264.h"

#include "modules/video_coding/codecs/h264/openh264_encoder.h"

namespace avp {

std::unique_ptr<H264Encoder> H264Encoder::Create() {
  return std::make_unique<OpenH264Encoder>();
}

}  // namespace avp
