/*
 * encoded_image.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef ENCODED_IMAGE_H
#define ENCODED_IMAGE_H

#include <optional>

#include <stdint.h>

#include "base/checks.h"
#include "base/types.h"

namespace avp {

class EncodedImageBufferInterface {
 public:
  virtual const uint8_t* Data() const = 0;
  // this non-const data method.
  virtual uint8_t* Data() = 0;
  virtual size_t Size() const = 0;
};

// Basic implementation of EncodedImageBufferInterface.
class EncodedImageBuffer : public EncodedImageBufferInterface {
 protected:
  // for private construct
  struct protect_parameter {
    explicit protect_parameter() {}
  };

 public:
  static std::shared_ptr<EncodedImageBuffer> Create() { return Create(0); }
  static std::shared_ptr<EncodedImageBuffer> Create(size_t size);
  static std::shared_ptr<EncodedImageBuffer> Create(const uint8_t* data,
                                                    size_t size);

  explicit EncodedImageBuffer(size_t size,
                              protect_parameter p = protect_parameter());
  EncodedImageBuffer(const uint8_t* data,
                     size_t size,
                     protect_parameter p = protect_parameter());
  virtual ~EncodedImageBuffer();

  const uint8_t* Data() const override;
  uint8_t* Data() override;
  size_t Size() const override;

 protected:
  size_t size_;
  uint8_t* buffer_;
};

class EncodedImage {
 public:
  EncodedImage();
  EncodedImage(EncodedImage&&);
  EncodedImage(const EncodedImage&);
  EncodedImage& operator=(EncodedImage&&);
  EncodedImage& operator=(const EncodedImage&);

  ~EncodedImage();

  void SetTimestamp(uint32_t timestamp) { timestamp_us_ = timestamp; }

  void SetEncodeTime(int64_t encode_start_ms, int64_t encode_finish_ms);

  size_t Size() const { return size_; }

  void SetSize(size_t new_size) {
    // Allow set_size(0) even if we have no buffer.
    DCHECK_LE(new_size, new_size == 0 ? 0 : Capacity());
    size_ = new_size;
  }

  void SetEncodedData(std::shared_ptr<EncodedImageBuffer> encoded_data) {
    encoded_data_ = std::move(encoded_data);
    size_ = encoded_data_->Size();
  }

  void ClearEncodedData() {
    encoded_data_ = nullptr;
    size_ = 0;
  }

  std::shared_ptr<EncodedImageBufferInterface> GetEncodedData() const {
    return encoded_data_;
  }

  const uint8_t* Data() const {
    return encoded_data_ ? encoded_data_->Data() : nullptr;
  }

  uint32_t encoded_width_ = 0;
  uint32_t encoded_height_ = 0;
  int64_t capture_time_ms_ = 0;
  int qp_ = -1;  // Quantizer value.

 private:
  size_t Capacity() const { return encoded_data_ ? encoded_data_->Size() : 0; }

  std::shared_ptr<EncodedImageBufferInterface> encoded_data_;
  size_t size_ = 0;  // Size of encoded frame data.
  uint32_t timestamp_us_ = 0;
};

}  // namespace avp

#endif /* !ENCODED_IMAGE_H */