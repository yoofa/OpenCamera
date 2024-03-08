/*
 * builtin_video_encoder_factory.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef BUILTIN_VIDEO_ENCODER_FACTORY_H
#define BUILTIN_VIDEO_ENCODER_FACTORY_H

#include "api/video_codecs/video_encoder_factory.h"

namespace ave {

std::unique_ptr<VideoEncoderFactory> CreateBuiltinVideoEncoderFactory();

}  // namespace ave

#endif /* !BUILTIN_VIDEO_ENCODER_FACTORY_H */
