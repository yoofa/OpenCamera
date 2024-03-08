/*
 * i420_buffer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "i420_buffer.h"

#include <memory>

#include "base/checks.h"
#include "third_party/libyuv/include/libyuv.h"

namespace ave {

namespace {
// Aligning pointer to 64 bytes for improved performance, e.g. use SIMD.
static const size_t kBufferAlignment = 64;

int I420DataSize(size_t height,
                 size_t stride_y,
                 size_t stride_u,
                 size_t stride_v) {
  return stride_y * height + (stride_u + stride_v) * ((height + 1) / 2);
}
}  // namespace

I420Buffer::I420Buffer(size_t width, size_t height, protect_parameter)
    : I420Buffer(width, height, width, (width + 1) / 2, (width + 1) / 2) {}

I420Buffer::I420Buffer(size_t width,
                       size_t height,
                       size_t stride_y,
                       size_t stride_u,
                       size_t stride_v,
                       protect_parameter)
    : width_(width),
      height_(height),
      stride_y_(stride_y),
      stride_u_(stride_u),
      stride_v_(stride_v),
      data_(static_cast<uint8_t*>(base::AlignedMalloc(
          I420DataSize(height, stride_y, stride_u, stride_v),
          kBufferAlignment))) {
  AVE_DCHECK_GT(width, 0);
  AVE_DCHECK_GT(height, 0);
  AVE_DCHECK_GE(stride_y, width);
  AVE_DCHECK_GE(stride_u, (width + 1) / 2);
  AVE_DCHECK_GE(stride_v, (width + 1) / 2);
}

I420Buffer::~I420Buffer() {}

// static
std::shared_ptr<I420Buffer> I420Buffer::Create(size_t width, size_t height) {
  return std::make_shared<I420Buffer>(width, height);
}

// static
std::shared_ptr<I420Buffer> I420Buffer::Create(size_t width,
                                               size_t height,
                                               size_t stride_y,
                                               size_t stride_u,
                                               size_t stride_v) {
  return std::make_shared<I420Buffer>(width, height, stride_y, stride_u,
                                      stride_v);
}

std::shared_ptr<I420Buffer> I420Buffer::Copy(
    const I420BufferInterface& source) {
  return Copy(source.width(), source.height(), source.DataY(), source.StrideY(),
              source.DataU(), source.StrideU(), source.DataV(),
              source.StrideV());
}

std::shared_ptr<I420Buffer> I420Buffer::Copy(size_t width,
                                             size_t height,
                                             const uint8_t* data_y,
                                             size_t stride_y,
                                             const uint8_t* data_u,
                                             size_t stride_u,
                                             const uint8_t* data_v,
                                             size_t stride_v) {
  std::shared_ptr<I420Buffer> buffer =
      Create(width, height, stride_y, stride_u, stride_v);

  // copy data to buffer
  AVE_CHECK_EQ(0, libyuv::I420Copy(data_y, stride_y, data_u, stride_u, data_v,
                                   stride_v, buffer->MutableDataY(),
                                   buffer->StrideY(), buffer->MutableDataU(),
                                   buffer->StrideU(), buffer->MutableDataV(),
                                   buffer->StrideV(), width, height));

  return buffer;
}

size_t I420Buffer::width() const {
  return width_;
}

size_t I420Buffer::height() const {
  return height_;
}

const uint8_t* I420Buffer::DataY() const {
  return data_.get();
}

const uint8_t* I420Buffer::DataU() const {
  return data_.get() + stride_y_ * height_;
}

const uint8_t* I420Buffer::DataV() const {
  return data_.get() + stride_y_ * height_ + stride_u_ * ((height_ + 1) / 2);
}

size_t I420Buffer::StrideY() const {
  return stride_y_;
}

size_t I420Buffer::StrideU() const {
  return stride_u_;
}

size_t I420Buffer::StrideV() const {
  return stride_v_;
}

uint8_t* I420Buffer::MutableDataY() {
  return const_cast<uint8_t*>(DataY());
}
uint8_t* I420Buffer::MutableDataU() {
  return const_cast<uint8_t*>(DataU());
}
uint8_t* I420Buffer::MutableDataV() {
  return const_cast<uint8_t*>(DataV());
}

void I420Buffer::SetBlack(I420Buffer* buffer) {
  AVE_CHECK(libyuv::I420Rect(buffer->MutableDataY(), buffer->StrideY(),
                             buffer->MutableDataU(), buffer->StrideU(),
                             buffer->MutableDataV(), buffer->StrideV(), 0, 0,
                             buffer->width(), buffer->height(), 0, 128,
                             128) == 0);
}

void I420Buffer::CropAndScaleFrom(const I420BufferInterface& src,
                                  size_t offset_x,
                                  size_t offset_y,
                                  size_t crop_width,
                                  size_t crop_height) {
  AVE_CHECK_LE(crop_width, src.width());
  AVE_CHECK_LE(crop_height, src.height());
  AVE_CHECK_LE(crop_width + offset_x, src.width());
  AVE_CHECK_LE(crop_height + offset_y, src.height());
  AVE_CHECK_GE(offset_x, 0);
  AVE_CHECK_GE(offset_y, 0);

  // Make sure offset is even so that u/v plane becomes aligned.
  const size_t uv_offset_x = offset_x / 2;
  const size_t uv_offset_y = offset_y / 2;
  offset_x = uv_offset_x * 2;
  offset_y = uv_offset_y * 2;

  const uint8_t* y_plane = src.DataY() + src.StrideY() * offset_y + offset_x;
  const uint8_t* u_plane =
      src.DataU() + src.StrideU() * uv_offset_y + uv_offset_x;
  const uint8_t* v_plane =
      src.DataV() + src.StrideV() * uv_offset_y + uv_offset_x;
  size_t res =
      libyuv::I420Scale(y_plane, src.StrideY(), u_plane, src.StrideU(), v_plane,
                        src.StrideV(), crop_width, crop_height, MutableDataY(),
                        StrideY(), MutableDataU(), StrideU(), MutableDataV(),
                        StrideV(), width(), height(), libyuv::kFilterBox);

  AVE_CHECK_EQ(res, 0);
}

void I420Buffer::CropAndScaleFrom(const I420BufferInterface& src) {
  const int crop_width =
      height() > 0 ? std::min(src.width(), width() * src.height() / height())
                   : src.width();
  const int crop_height =
      width() > 0 ? std::min(src.height(), height() * src.width() / width())
                  : src.height();

  CropAndScaleFrom(src, (src.width() - crop_width) / 2,
                   (src.height() - crop_height) / 2, crop_width, crop_height);
}

void I420Buffer::ScaleFrom(const I420BufferInterface& src) {
  CropAndScaleFrom(src, 0, 0, src.width(), src.height());
}

}  // namespace ave
