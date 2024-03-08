/*
 * video_frame_buffer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_frame_buffer.h"

#include <memory>

#include "api/video/i420_buffer.h"
#include "api/video/nv12_buffer.h"
#include "api/video/yuyv_buffer.h"

namespace ave {

/** VideoFrameBuffer **/

const I420BufferInterface* VideoFrameBuffer::GetI420() const {
  return nullptr;
}

std::shared_ptr<VideoFrameBuffer> VideoFrameBuffer::CropAndScale(
    size_t offset_x,
    size_t offset_y,
    size_t crop_width,
    size_t crop_height,
    size_t scaled_width,
    size_t scaled_height) {
  auto result = I420Buffer::Create(scaled_width, scaled_height);
  result->CropAndScaleFrom(*this->ToI420(), offset_x, offset_y, crop_width,
                           crop_height);
  return result;
}

/** I420BufferInterface **/

VideoFrameBuffer::Type I420BufferInterface::type() const {
  return Type::kNormal;
}

VideoFrameBuffer::PixelFormat I420BufferInterface::pixel_format() const {
  return PixelFormat::kI420;
}

size_t I420BufferInterface::ChromaWidth() const {
  return (width() + 1) / 2;
}

size_t I420BufferInterface::ChromaHeight() const {
  return (height() + 1) / 2;
}

std::shared_ptr<I420BufferInterface> I420BufferInterface::ToI420() {
  return I420BufferInterface::shared_from_this();
}

const I420BufferInterface* I420BufferInterface::GetI420() const {
  return this;
}

/** NV12BufferInterface **/

VideoFrameBuffer::Type NV12BufferInterface::type() const {
  return Type::kNormal;
}

VideoFrameBuffer::PixelFormat NV12BufferInterface::pixel_format() const {
  return PixelFormat::kNV12;
}

size_t NV12BufferInterface::ChromaWidth() const {
  return (width() + 1) / 2;
}

size_t NV12BufferInterface::ChromaHeight() const {
  return (height() + 1) / 2;
}

std::shared_ptr<VideoFrameBuffer> NV12BufferInterface::CropAndScale(
    size_t offset_x,
    size_t offset_y,
    size_t crop_width,
    size_t crop_height,
    size_t scaled_width,
    size_t scaled_height) {
  auto result = NV12Buffer::Create(scaled_width, scaled_height);
  result->CropAndScaleFrom(*this, offset_x, offset_y, crop_width, crop_height);
  return result;
}

/** YUYVBufferInterface **/
VideoFrameBuffer::Type YUYVBufferInterface::type() const {
  return Type::kNormal;
}

VideoFrameBuffer::PixelFormat YUYVBufferInterface::pixel_format() const {
  return PixelFormat::kYUY2;
}

std::shared_ptr<VideoFrameBuffer> YUYVBufferInterface::CropAndScale(
    size_t offset_x,
    size_t offset_y,
    size_t crop_width,
    size_t crop_height,
    size_t scaled_width,
    size_t scaled_height) {
  auto result = YUYVBuffer::Create(scaled_width, scaled_height);
  result->CropAndScaleFrom(*this, offset_x, offset_y, crop_width, crop_height);
  return result;
}

}  // namespace ave
