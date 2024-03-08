/*
 * default_video_source_factory.h
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef DEFAULT_VIDEO_SOURCE_FACTORY_H
#define DEFAULT_VIDEO_SOURCE_FACTORY_H

#include "media/camera_info/video_source_factory.h"

namespace ave {

std::unique_ptr<VideoSourceFactory> CreateDefaultVideoSourceFactory();

}  // namespace ave

#endif /* !DEFAULT_VIDEO_SOURCE_FACTORY_H */
