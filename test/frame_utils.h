/*
 * frame_utils.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef FRAME_UTILS_H
#define FRAME_UTILS_H

#include <stdint.h>

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"

namespace ave {
namespace test {

bool EqualPlane(const uint8_t* data1,
                const uint8_t* data2,
                int stride1,
                int stride2,
                int width,
                int height);

static inline bool EqualPlane(const uint8_t* data1,
                              const uint8_t* data2,
                              int stride,
                              int width,
                              int height) {
  return EqualPlane(data1, data2, stride, stride, width, height);
}

bool FramesEqual(const VideoFrame& f1, const VideoFrame& f2);

bool FrameBufsEqual(const std::shared_ptr<VideoFrameBuffer>& f1,
                    const std::shared_ptr<VideoFrameBuffer>& f2);

std::shared_ptr<I420Buffer> ReadI420Buffer(int width, int height, FILE*);

}  // namespace test
}  // namespace ave

#endif /* !FRAME_UTILS_H */
