/*
 * yuyv_buffer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "yuyv_buffer.h"
#include <cstring>

#include "api/video/i420_buffer.h"
#include "base/checks.h"
#include "third_party/libyuv/include/libyuv.h"
#include "third_party/libyuv/include/libyuv/convert.h"
#include "third_party/libyuv/include/libyuv/planar_functions.h"

namespace avp {

namespace {

static const int kBufferAlignment = 64;

size_t YUYVDataSize(size_t height, size_t stride_yuyv) {
  return stride_yuyv * height;
}

}  // namespace

YUYVBuffer::YUYVBuffer(size_t width, size_t height, protect_parameter)
    : YUYVBuffer(width, height, (width + width % 2) * 2) {}

YUYVBuffer::YUYVBuffer(size_t width,
                       size_t height,
                       size_t stride_yuyv,
                       protect_parameter)
    : width_(width),
      height_(height),
      stride_yuyv_(stride_yuyv),
      data_(static_cast<uint8_t*>(
          AlignedMalloc(YUYVDataSize(height_, stride_yuyv_),
                        kBufferAlignment))) {
  DCHECK_GT(width, 0);
  DCHECK_GT(height, 0);
  DCHECK_GE(stride_yuyv, width);
}

YUYVBuffer::~YUYVBuffer() = default;

// static
std::shared_ptr<YUYVBuffer> YUYVBuffer::Create(size_t width, size_t height) {
  return std::make_shared<YUYVBuffer>(width, height);
}

// static
std::shared_ptr<YUYVBuffer> YUYVBuffer::Create(size_t width,
                                               size_t height,
                                               size_t stride_yuyv) {
  return std::make_shared<YUYVBuffer>(width, height, stride_yuyv);
}

// static
std::shared_ptr<YUYVBuffer> YUYVBuffer::Copy(
    const I420BufferInterface& i420_buffer) {
  std::shared_ptr<YUYVBuffer> buffer =
      YUYVBuffer::Create(i420_buffer.width(), i420_buffer.height());
  // i420 -> yuyv
  libyuv::I420ToYUY2(i420_buffer.DataY(), i420_buffer.StrideY(),
                     i420_buffer.DataU(), i420_buffer.StrideU(),
                     i420_buffer.DataV(), i420_buffer.StrideV(),
                     buffer->MutableData(), buffer->Stride(), buffer->width(),
                     buffer->height());
  return buffer;
}

// static
std::shared_ptr<YUYVBuffer> YUYVBuffer::Copy(const uint8_t* data,
                                             size_t stride_yuyv,
                                             size_t width,
                                             size_t height) {
  std::shared_ptr<YUYVBuffer> buffer = YUYVBuffer::Create(width, height);
  // copy packed yuyv as a single plane
  libyuv::CopyPlane(data, stride_yuyv, buffer->MutableData(), buffer->Stride(),
                    width * 2, height);
  return buffer;
}

std::shared_ptr<I420BufferInterface> YUYVBuffer::ToI420() {
  std::shared_ptr<I420Buffer> buffer = I420Buffer::Create(width_, height_);
  // yuyv -> i420
  libyuv::YUY2ToI420(Data(), Stride(), buffer->MutableDataY(),
                     buffer->StrideY(), buffer->MutableDataU(),
                     buffer->StrideU(), buffer->MutableDataV(),
                     buffer->StrideV(), width_, height_);
  return buffer;
}

size_t YUYVBuffer::width() const {
  return width_;
}

size_t YUYVBuffer::height() const {
  return height_;
}

size_t YUYVBuffer::Stride() const {
  return stride_yuyv_;
}

const uint8_t* YUYVBuffer::Data() const {
  return data_.get();
}

uint8_t* YUYVBuffer::MutableData() {
  return data_.get();
}

void YUYVBuffer::CropAndScaleFrom(const YUYVBufferInterface& src,
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
  // TODO(youfa) not support now
}

}  // namespace avp
