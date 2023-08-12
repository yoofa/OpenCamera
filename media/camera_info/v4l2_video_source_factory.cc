/*
 * v4l2_video_source_factory.cc
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "media/camera_info/default_video_source_factory.h"
#include "media/camera_info/v4l2_video_source.h"

namespace avp {

std::unique_ptr<VideoSourceFactory> CreateDefaultVideoSourceFactory() {
  return CreateV4l2VideoSourceFactory();
}

}  // namespace avp
