/*
 * frame_utils.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "frame_utils.h"

namespace avp {
namespace test {

bool EqualPlane(const uint8_t* data1,
                const uint8_t* data2,
                int stride1,
                int stride2,
                int width,
                int height) {
  for (int i = 0; i < height; ++i) {
    if (memcmp(data1, data2, width) != 0) {
      return false;
    }
    data1 += stride1;
    data2 += stride2;
  }
  return true;
}

bool FramesEqual(const VideoFrame& f1, const VideoFrame& f2) {
  if (f1.width() != f2.width() || f1.height() != f2.height()) {
    return false;
  }
  if (f1.timestamp_us() != f2.timestamp_us()) {
    return false;
  }
  if (!FrameBufsEqual(f1.video_frame_buffer(), f2.video_frame_buffer())) {
    return false;
  }
  return true;
}

bool FrameBufsEqual(const std::shared_ptr<VideoFrameBuffer>& f1,
                    const std::shared_ptr<VideoFrameBuffer>& f2) {
  if (f1 == f2) {
    return true;
  }
  // Exlude nullptr (except if both are nullptr, as above)
  if (!f1 || !f2) {
    return false;
  }

  if (f1->width() != f2->width() || f1->height() != f2->height() ||
      f1->type() != f2->type()) {
    return false;
  }

  auto f1_i420 = f1->ToI420();
  auto f2_i420 = f2->ToI420();
  return EqualPlane(f1_i420->DataY(), f2_i420->DataY(), f1_i420->StrideY(),
                    f2_i420->StrideY(), f1_i420->width(), f1_i420->height()) &&
         EqualPlane(f1_i420->DataU(), f2_i420->DataU(), f1_i420->StrideU(),
                    f2_i420->StrideU(), f1_i420->ChromaWidth(),
                    f1_i420->ChromaHeight()) &&
         EqualPlane(f1_i420->DataV(), f2_i420->DataV(), f1_i420->StrideV(),
                    f2_i420->StrideV(), f1_i420->ChromaWidth(),
                    f1_i420->ChromaHeight());
}

std::shared_ptr<I420Buffer> ReadI420Buffer(int width, int height, FILE* f) {
  int half_width = (width + 1) / 2;
  auto buffer(
      // Explicit stride, no padding between rows.
      I420Buffer::Create(width, height, width, half_width, half_width));
  size_t size_y = static_cast<size_t>(width) * height;
  size_t size_uv = static_cast<size_t>(half_width) * ((height + 1) / 2);

  if (fread(buffer->MutableDataY(), 1, size_y, f) < size_y)
    return nullptr;
  if (fread(buffer->MutableDataU(), 1, size_uv, f) < size_uv)
    return nullptr;
  if (fread(buffer->MutableDataV(), 1, size_uv, f) < size_uv)
    return nullptr;
  return buffer;
}

}  // namespace test
}  // namespace avp
