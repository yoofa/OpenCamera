/*
 * video_codec_properity_initializer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_CODEC_PROPERITY_INITIALIZER_H
#define VIDEO_CODEC_PROPERITY_INITIALIZER_H
#include "api/video_codecs/video_encoder_config.h"

namespace ave {

std::vector<VideoStreamConfig> CreateVideoStreamConfig(
    size_t width,
    size_t height,
    const VideoEncoderConfig& video_encoder_config);

bool SetupVideoCodecProperity(
    const VideoEncoderConfig& video_encoder_config,
    const std::vector<VideoStreamConfig>& stream_configs,
    VideoCodecProperty* codec_properity);

}  // namespace ave

#endif /* !VIDEO_CODEC_PROPERITY_INITIALIZER_H */
