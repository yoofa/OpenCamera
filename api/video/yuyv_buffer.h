/*
 * yuyv_buffer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef YUYV_BUFFER_H
#define YUYV_BUFFER_H

#include <memory>

#include "api/video/video_frame_buffer.h"
#include "base/memory/aligned_memory.h"

namespace avp {

class YUYVBuffer : public YUYVBufferInterface {
 protected:
  // for private construct
  struct protect_parameter {
    explicit protect_parameter() {}
  };

 public:
  static std::shared_ptr<YUYVBuffer> Create(size_t width, size_t height);
  static std::shared_ptr<YUYVBuffer> Create(size_t width,
                                            size_t height,
                                            size_t stride_yuyv);

  // copy data from i420_buffer
  static std::shared_ptr<YUYVBuffer> Copy(
      const I420BufferInterface& i420_buffer);

  // copy data from external memory
  static std::shared_ptr<YUYVBuffer> Copy(const uint8_t* data,
                                          size_t stride_yuyv,
                                          size_t width,
                                          size_t height);

  YUYVBuffer(size_t width,
             size_t height,
             protect_parameter = protect_parameter());

  YUYVBuffer(size_t width,
             size_t height,
             size_t stride_yuyv,
             protect_parameter = protect_parameter());

  ~YUYVBuffer() override;

  size_t width() const override;
  size_t height() const override;

  const uint8_t* Data() const override;
  size_t Stride() const override;

  uint8_t* MutableData();

  std::shared_ptr<I420BufferInterface> ToI420() override;

  void CropAndScaleFrom(const YUYVBufferInterface& src,
                        size_t offset_x,
                        size_t offset_y,
                        size_t crop_width,
                        size_t crop_height);

 private:
  const size_t width_;
  const size_t height_;
  const size_t stride_yuyv_;
  const std::unique_ptr<uint8_t, AlignedFreeDeleter> data_;
};

}  // namespace avp

#endif /* !YUYV_BUFFER_H */
