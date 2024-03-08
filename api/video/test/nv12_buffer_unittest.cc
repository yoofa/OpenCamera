/*
 * nv12_buffer_unittest.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include <memory>

#include "api/video/i420_buffer.h"
#include "api/video/nv12_buffer.h"
#include "base/types.h"
#include "gtest/gtest.h"
#include "test/frame_utils.h"
#include "test/gtest.h"

namespace ave {

namespace {
int GetY(std::shared_ptr<NV12BufferInterface>& buf, size_t col, size_t row) {
  return buf->DataY()[row * buf->StrideY() + col];
}

int GetU(std::shared_ptr<NV12BufferInterface>& buf, size_t col, size_t row) {
  return buf->DataUV()[(row / 2) * buf->StrideUV() + (col / 2) * 2];
}

int GetV(std::shared_ptr<NV12BufferInterface>& buf, size_t col, size_t row) {
  return buf->DataUV()[(row / 2) * buf->StrideUV() + (col / 2) * 2 + 1];
}

void FillNV12Buffer(std::shared_ptr<NV12Buffer>& buf) {
  const uint8_t Y = 1;
  const uint8_t U = 2;
  const uint8_t V = 3;
  for (size_t row = 0; row < buf->height(); ++row) {
    for (size_t col = 0; col < buf->width(); ++col) {
      buf->MutableDataY()[row * buf->StrideY() + col] = Y;
    }
  }
  // Fill interleaving UV values.
  for (size_t row = 0; row < buf->ChromaHeight(); row++) {
    for (size_t col = 0; col < buf->StrideUV(); col += 2) {
      int uv_index = row * buf->StrideUV() + col;
      buf->MutableDataUV()[uv_index] = U;
      buf->MutableDataUV()[uv_index + 1] = V;
    }
  }
}

}  // namespace

TEST(NV12BufferTest, InitialData) {
  constexpr size_t width = 3;
  constexpr size_t height = 4;
  constexpr size_t stride_y = 3;
  constexpr size_t stride_uv = 4;

  auto nv12_buffer = NV12Buffer::Create(width, height, stride_y, stride_uv);
  EXPECT_EQ(width, nv12_buffer->width());
  EXPECT_EQ(height, nv12_buffer->height());
  EXPECT_EQ(stride_y, nv12_buffer->StrideY());
  EXPECT_EQ(stride_uv, nv12_buffer->StrideUV());
  EXPECT_EQ((unsigned long)2, nv12_buffer->ChromaWidth());
  EXPECT_EQ((unsigned long)2, nv12_buffer->ChromaHeight());
}

TEST(NV12BufferTest, ReadPixels) {
  constexpr size_t width = 3;
  constexpr size_t height = 3;

  std::shared_ptr<NV12Buffer> nv12_buffer = NV12Buffer::Create(width, height);
  auto buf = std::dynamic_pointer_cast<NV12BufferInterface>(nv12_buffer);
  FillNV12Buffer(nv12_buffer);
  for (size_t row = 0; row < height; ++row) {
    for (size_t col = 0; col < width; ++col) {
      EXPECT_EQ(1, GetY(buf, col, row));
      EXPECT_EQ(2, GetU(buf, col, row));
      EXPECT_EQ(3, GetV(buf, col, row));
    }
  }
}

TEST(NV12BufferTest, ToI420) {
  constexpr size_t width = 3;
  constexpr size_t height = 3;
  constexpr size_t size_y = width * height;
  constexpr size_t size_u = (width + 1) / 2 * ((height + 1) / 2);
  constexpr size_t size_v = (width + 1) / 2 * ((height + 1) / 2);
  auto ref_i420_buffer = I420Buffer::Create(width, height);
  // I420: 1, 2, 3
  memset(ref_i420_buffer->MutableDataY(), 1, size_y);
  memset(ref_i420_buffer->MutableDataU(), 2, size_u);
  memset(ref_i420_buffer->MutableDataV(), 3, size_v);

  std::shared_ptr<NV12Buffer> nv12_buffer = NV12Buffer::Create(width, height);
  auto buf = std::dynamic_pointer_cast<NV12BufferInterface>(nv12_buffer);
  FillNV12Buffer(nv12_buffer);

  auto i420_buffer = buf->ToI420();
  EXPECT_EQ(width, i420_buffer->width());
  EXPECT_EQ(height, i420_buffer->height());
  EXPECT_TRUE(test::FrameBufsEqual(ref_i420_buffer, i420_buffer));
}

}  // namespace ave
