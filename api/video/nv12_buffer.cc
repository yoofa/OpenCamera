/*
 * nv12_buffer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "nv12_buffer.h"

#include <memory>

#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "base/checks.h"
#include "third_party/libyuv/include/libyuv.h"
#include "third_party/libyuv/include/libyuv/planar_functions.h"

namespace avp {

namespace {

static const int kBufferAlignment = 64;

int NV12DataSize(int height, int stride_y, int stride_uv) {
  return stride_y * height + stride_uv * ((height + 1) / 2);
}

}  // namespace

NV12Buffer::NV12Buffer(size_t width, size_t height, protect_parameter)
    : NV12Buffer(width, height, width, width + width % 2) {}

NV12Buffer::NV12Buffer(size_t width,
                       size_t height,
                       size_t stride_y,
                       size_t stride_uv,
                       protect_parameter)
    : width_(width),
      height_(height),
      stride_y_(stride_y),
      stride_uv_(stride_uv),
      data_(static_cast<uint8_t*>(
          AlignedMalloc(NV12DataSize(height_, stride_y_, stride_uv),
                        kBufferAlignment))) {
  DCHECK_GT(width, 0);
  DCHECK_GT(height, 0);
  DCHECK_GE(stride_y, width);
  DCHECK_GE(stride_uv, (width + width % 2));
}

NV12Buffer::~NV12Buffer() = default;

// static
std::shared_ptr<NV12Buffer> NV12Buffer::Create(size_t width, size_t height) {
  return std::make_shared<NV12Buffer>(width, height);
}

// static
std::shared_ptr<NV12Buffer> NV12Buffer::Create(size_t width,
                                               size_t height,
                                               size_t stride_y,
                                               size_t stride_uv) {
  return std::make_shared<NV12Buffer>(width, height, stride_y, stride_uv);
}

// static
std::shared_ptr<NV12Buffer> NV12Buffer::Copy(
    const I420BufferInterface& i420_buffer) {
  std::shared_ptr<NV12Buffer> buffer =
      NV12Buffer::Create(i420_buffer.width(), i420_buffer.height());
  // i420 -> nv12
  libyuv::I420ToNV12(
      i420_buffer.DataY(), i420_buffer.StrideY(), i420_buffer.DataU(),
      i420_buffer.StrideU(), i420_buffer.DataV(), i420_buffer.StrideV(),
      buffer->MutableDataY(), buffer->StrideY(), buffer->MutableDataUV(),
      buffer->StrideUV(), buffer->width(), buffer->height());
  return buffer;
}

// static
std::shared_ptr<NV12Buffer> NV12Buffer::Copy(const uint8_t* data_y,
                                             size_t stride_y,
                                             const uint8_t* data_uv,
                                             size_t stride_uv,
                                             size_t width,
                                             size_t height) {
  std::shared_ptr<NV12Buffer> buffer = NV12Buffer::Create(width, height);
  libyuv::NV12Copy(data_y, stride_y, data_uv, stride_uv, buffer->MutableDataY(),
                   buffer->StrideY(), buffer->MutableDataUV(),
                   buffer->StrideUV(), width, height);
  return buffer;
}

std::shared_ptr<I420BufferInterface> NV12Buffer::ToI420() {
  std::shared_ptr<I420Buffer> buffer = I420Buffer::Create(width_, height_);
  // nv12 -> i420
  libyuv::NV12ToI420(
      DataY(), StrideY(), DataUV(), StrideUV(), buffer->MutableDataY(),
      buffer->StrideY(), buffer->MutableDataU(), buffer->StrideU(),
      buffer->MutableDataV(), buffer->StrideV(), width_, height_);
  return buffer;
}

size_t NV12Buffer::width() const {
  return width_;
}

size_t NV12Buffer::height() const {
  return height_;
}

size_t NV12Buffer::StrideY() const {
  return stride_y_;
}

size_t NV12Buffer::StrideUV() const {
  return stride_uv_;
}

const uint8_t* NV12Buffer::DataY() const {
  return data_.get();
}

const uint8_t* NV12Buffer::DataUV() const {
  return data_.get() + stride_y_ * height_;
}

uint8_t* NV12Buffer::MutableDataY() {
  return data_.get();
}

uint8_t* NV12Buffer::MutableDataUV() {
  return data_.get() + stride_y_ * height_;
}

void NV12Buffer::CropAndScaleFrom(const NV12BufferInterface& src,
                                  size_t offset_x,
                                  size_t offset_y,
                                  size_t crop_width,
                                  size_t crop_height) {
  CHECK_LE(crop_width, src.width());
  CHECK_LE(crop_height, src.height());
  CHECK_LE(crop_width + offset_x, src.width());
  CHECK_LE(crop_height + offset_y, src.height());
  CHECK_GE(offset_x, 0);
  CHECK_GE(offset_y, 0);

  // Make sure offset is even so that u/v plane becomes aligned.
  const int uv_offset_x = offset_x / 2;
  const int uv_offset_y = offset_y / 2;
  offset_x = uv_offset_x * 2;
  offset_y = uv_offset_y * 2;

  const uint8_t* y_plane = src.DataY() + src.StrideY() * offset_y + offset_x;
  const uint8_t* uv_plane =
      src.DataUV() + src.StrideUV() * uv_offset_y + uv_offset_x * 2;

  int res = libyuv::NV12Scale(y_plane, src.StrideY(), uv_plane, src.StrideUV(),
                              crop_width, crop_height, MutableDataY(),
                              StrideY(), MutableDataUV(), StrideUV(), width(),
                              height(), libyuv::kFilterBox);

  DCHECK_EQ(res, 0);
}

}  // namespace avp
