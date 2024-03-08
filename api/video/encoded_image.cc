/*
 * encoded_image.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "encoded_image.h"
#include <cstdint>

namespace ave {
EncodedImageBuffer::EncodedImageBuffer(size_t size, protect_parameter p)
    : size_(size), buffer_(static_cast<uint8_t*>(malloc(size))) {}

EncodedImageBuffer::EncodedImageBuffer(const uint8_t* data,
                                       size_t size,
                                       protect_parameter p)
    : EncodedImageBuffer(size) {
  memcpy(buffer_, data, size);
}

EncodedImageBuffer::~EncodedImageBuffer() {
  free(buffer_);
}

// static
std::shared_ptr<EncodedImageBuffer> EncodedImageBuffer::Create(size_t size) {
  return std::make_shared<EncodedImageBuffer>(size);
}

// static
std::shared_ptr<EncodedImageBuffer> EncodedImageBuffer::Create(
    const uint8_t* data,
    size_t size) {
  return std::make_shared<EncodedImageBuffer>(data, size);
}

const uint8_t* EncodedImageBuffer::Data() const {
  return buffer_;
}

uint8_t* EncodedImageBuffer::Data() {
  return buffer_;
}

size_t EncodedImageBuffer::Size() const {
  return size_;
}

EncodedImage::EncodedImage() = default;
EncodedImage::EncodedImage(EncodedImage&& rhs) = default;
EncodedImage::EncodedImage(const EncodedImage& rhs) = default;
EncodedImage& EncodedImage::operator=(EncodedImage&& rhs) = default;
EncodedImage& EncodedImage::operator=(const EncodedImage& rhs) = default;
EncodedImage::~EncodedImage() = default;

}  // namespace ave
