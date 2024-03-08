/*
 * i420_buffer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef I420_BUFFER_H
#define I420_BUFFER_H

#include "api/video/video_frame_buffer.h"
#include "base/memory/aligned_memory.h"

namespace ave {

// Plain I420 buffer in standard memory.
class I420Buffer : public I420BufferInterface {
 protected:
  // for private construct
  struct protect_parameter {
    explicit protect_parameter() {}
  };

 public:
  static std::shared_ptr<I420Buffer> Create(size_t width, size_t height);
  static std::shared_ptr<I420Buffer> Create(size_t width,
                                            size_t height,
                                            size_t stride_y,
                                            size_t stride_u,
                                            size_t stride_v);

  // Create a new buffer and copy the pixel data.
  static std::shared_ptr<I420Buffer> Copy(const I420BufferInterface& buffer);

  static std::shared_ptr<I420Buffer> Copy(size_t width,
                                          size_t height,
                                          const uint8_t* data_y,
                                          size_t stride_y,
                                          const uint8_t* data_u,
                                          size_t stride_u,
                                          const uint8_t* data_v,
                                          size_t stride_v);

  I420Buffer(size_t width,
             size_t height,
             protect_parameter = protect_parameter());
  I420Buffer(size_t width,
             size_t height,
             size_t stride_y,
             size_t stride_u,
             size_t stride_v,
             protect_parameter = protect_parameter());

  ~I420Buffer() override;

  // Sets the buffer to all black.
  static void SetBlack(I420Buffer* buffer);

  size_t width() const override;
  size_t height() const override;
  const uint8_t* DataY() const override;
  const uint8_t* DataU() const override;
  const uint8_t* DataV() const override;

  size_t StrideY() const override;
  size_t StrideU() const override;
  size_t StrideV() const override;

  uint8_t* MutableDataY();
  uint8_t* MutableDataU();
  uint8_t* MutableDataV();
  // Scale the cropped area of `src` to the size of `this` buffer, and
  // write the result into `this`.
  void CropAndScaleFrom(const I420BufferInterface& src,
                        size_t offset_x,
                        size_t offset_y,
                        size_t crop_width,
                        size_t crop_height);

  // The common case of a center crop, when needed to adjust the
  // aspect ratio without distorting the image.
  void CropAndScaleFrom(const I420BufferInterface& src);

  // Scale all of `src` to the size of `this` buffer, with no cropping.
  void ScaleFrom(const I420BufferInterface& src);

 private:
  const size_t width_;
  const size_t height_;
  const size_t stride_y_;
  const size_t stride_u_;
  const size_t stride_v_;
  const std::unique_ptr<uint8_t, base::AlignedFreeDeleter> data_;
};

}  // namespace ave

#endif /* !I420_BUFFER_H */
