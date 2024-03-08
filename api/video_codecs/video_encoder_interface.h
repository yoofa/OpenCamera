/*
 * video_encoder_interface.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_ENCODER_INTERFACE_H
#define VIDEO_ENCODER_INTERFACE_H

#include <memory>

#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
namespace ave {

class VideoEncoderInterface
    : public VideoSinkInterface<std::shared_ptr<VideoFrame>> {
 public:
};

}  // namespace ave

#endif /* !VIDEO_ENCODER_INTERFACE_H */
