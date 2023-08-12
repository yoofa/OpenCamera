/*
 * nv12_buffer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef NV12_BUFFER_H
#define NV12_BUFFER_H

#include "api/video/video_frame_buffer.h"
#include "base/memory/aligned_memory.h"

namespace avp {

// NV12 is a biplanar encoding format, with full-resolution Y and
// half-resolution interleved UV. More information can be found at
// http://msdn.microsoft.com/library/windows/desktop/dd206750.aspx#nv12.
class NV12Buffer : public NV12BufferInterface {
 protected:
  // for private construct
  struct protect_parameter {
    explicit protect_parameter() {}
  };

 public:
  static std::shared_ptr<NV12Buffer> Create(size_t width, size_t height);
  static std::shared_ptr<NV12Buffer> Create(size_t width,
                                            size_t height,
                                            size_t stride_y,
                                            size_t stride_uv);
  // copy data from i420_buffer
  static std::shared_ptr<NV12Buffer> Copy(
      const I420BufferInterface& i420_buffer);

  // copy data from external memory
  static std::shared_ptr<NV12Buffer> Copy(const uint8_t* data_y,
                                          size_t stride_y,
                                          const uint8_t* data_uv,
                                          size_t stride_uv,
                                          size_t width,
                                          size_t height);

  NV12Buffer(size_t width,
             size_t height,
             protect_parameter = protect_parameter());
  NV12Buffer(size_t width,
             size_t height,
             size_t stride_y,
             size_t stride_uv,
             protect_parameter = protect_parameter());

  ~NV12Buffer() override;

  std::shared_ptr<I420BufferInterface> ToI420() override;

  size_t width() const override;
  size_t height() const override;

  size_t StrideY() const override;
  size_t StrideUV() const override;

  const uint8_t* DataY() const override;
  const uint8_t* DataUV() const override;

  uint8_t* MutableDataY();
  uint8_t* MutableDataUV();

  // Scale the cropped area of `src` to the size of `this` buffer, and
  // write the result into `this`.
  void CropAndScaleFrom(const NV12BufferInterface& src,
                        size_t offset_x,
                        size_t offset_y,
                        size_t crop_width,
                        size_t crop_height);

 private:
  const size_t width_;
  const size_t height_;
  const size_t stride_y_;
  const size_t stride_uv_;
  const std::unique_ptr<uint8_t, AlignedFreeDeleter> data_;
};

}  // namespace avp

#endif /* !NV12_BUFFER_H */
