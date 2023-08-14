/*
 * video_encoder.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_encoder.h"

#include "base/types.h"
#include "common/video_codec_property.h"

namespace avp {

status_t VideoEncoder::InitEncoder(const VideoCodecProperty& codec_settings,
                                   const Settings& settings) {
  return OK;
}
}  // namespace avp
