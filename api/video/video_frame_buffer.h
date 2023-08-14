/*
 * video_frame_buffer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_FRAME_BUFFER_H
#define VIDEO_FRAME_BUFFER_H

#include <iostream>
#include <memory>
#include "base/types.h"

namespace avp {

class I420BufferInterface;

class VideoFrameBuffer {
 public:
  // buffer type
  enum class Type {
    kNormal,
    kTexture,
    kHardware,
    kPrivate,
  };

  static std::string TypeToString(Type type) {
    switch (type) {
      case Type::kNormal:
        return "Normal";
      case Type::kTexture:
        return "Texture";
      case Type::kHardware:
        return "Hardware";
      case Type::kPrivate:
        return "Private";
      default:
        return "Unknown";
    }
  }

  // pixel format
  enum class PixelFormat : int32_t {
    kUnknown = -1,
    kI420,
    kI420A,
    kI444,
    kIYUV,
    kRGB24,
    kABGR,
    kARGB,
    kARGB4444,
    kRGB565,
    kARGB1555,
    kYUY2,
    kYV12,
    kUYVY,
    kMJPEG,
    kNV21,
    kNV12,
    kBGRA,
    kLast = kBGRA,
  };

  // buffer type
  virtual Type type() const = 0;

  // pixel format
  virtual PixelFormat pixel_format() const = 0;

  // width and height
  virtual size_t width() const = 0;
  virtual size_t height() const = 0;

  virtual std::shared_ptr<I420BufferInterface> ToI420() = 0;
  virtual const I420BufferInterface* GetI420() const;

  virtual std::shared_ptr<VideoFrameBuffer> CropAndScale(size_t offset_x,
                                                         size_t offset_y,
                                                         size_t crop_width,
                                                         size_t crop_height,
                                                         size_t scaled_width,
                                                         size_t scaled_height);

 protected:
  virtual ~VideoFrameBuffer() = default;
};

// This interface represents planar formats.
class PlanarYuvBuffer : public VideoFrameBuffer {
 public:
  virtual size_t ChromaWidth() const = 0;
  virtual size_t ChromaHeight() const = 0;

  // Returns the number of steps(in terms of Data*() return type) between
  // successive rows for a given plane.
  virtual size_t StrideY() const = 0;
  virtual size_t StrideU() const = 0;
  virtual size_t StrideV() const = 0;

 protected:
  ~PlanarYuvBuffer() override {}
};
// This interface represents 8-bit color depth formats: PixelFormat::kI420,
// PixelFormat::kI420A and PixelFormat::kI444.
class PlanarYuv8Buffer : public PlanarYuvBuffer {
 public:
  // Returns pointer to the pixel data for a given plane. The memory is owned by
  // the VideoFrameBuffer object and must not be freed by the caller.
  virtual const uint8_t* DataY() const = 0;
  virtual const uint8_t* DataU() const = 0;
  virtual const uint8_t* DataV() const = 0;

 protected:
  ~PlanarYuv8Buffer() override {}
};

class I420BufferInterface
    : public PlanarYuv8Buffer,
      public std::enable_shared_from_this<I420BufferInterface> {
 public:
  // VideoFrameBuffer
  Type type() const override;
  PixelFormat pixel_format() const final override;

  // PlanarYuvBuffer
  size_t ChromaWidth() const final override;
  size_t ChromaHeight() const final override;

  std::shared_ptr<I420BufferInterface> ToI420() final;
  const I420BufferInterface* GetI420() const final;

 protected:
  ~I420BufferInterface() override {}
};

class BiplanarYuvBuffer : public VideoFrameBuffer {
 public:
  virtual size_t ChromaWidth() const = 0;
  virtual size_t ChromaHeight() const = 0;

  // Returns the number of steps(in terms of Data*() return type) between
  // successive rows for a given plane.
  virtual size_t StrideY() const = 0;
  virtual size_t StrideUV() const = 0;

 protected:
  ~BiplanarYuvBuffer() override {}
};

class BiplanarYuv8Buffer : public BiplanarYuvBuffer {
 public:
  virtual const uint8_t* DataY() const = 0;
  virtual const uint8_t* DataUV() const = 0;

 protected:
  ~BiplanarYuv8Buffer() override {}
};

// Represents PixelFormat::kNV12. NV12 is full resolution Y and half-resolution
// interleved UV.
class NV12BufferInterface : public BiplanarYuv8Buffer {
 public:
  Type type() const override;
  PixelFormat pixel_format() const final override;

  size_t ChromaWidth() const final;
  size_t ChromaHeight() const final;

  std::shared_ptr<VideoFrameBuffer> CropAndScale(size_t offset_x,
                                                 size_t offset_y,
                                                 size_t crop_width,
                                                 size_t crop_height,
                                                 size_t scaled_width,
                                                 size_t scaled_height) override;

 protected:
  ~NV12BufferInterface() override {}
};

class Packed8Buffer : public VideoFrameBuffer {
 public:
  virtual const uint8_t* Data() const = 0;
  virtual size_t Stride() const = 0;

 protected:
  ~Packed8Buffer() override {}
};

class YUYVBufferInterface : public Packed8Buffer {
 public:
  Type type() const override;
  PixelFormat pixel_format() const final override;

  std::shared_ptr<VideoFrameBuffer> CropAndScale(size_t offset_x,
                                                 size_t offset_y,
                                                 size_t crop_width,
                                                 size_t crop_height,
                                                 size_t scaled_width,
                                                 size_t scaled_height) override;

 protected:
  ~YUYVBufferInterface() override {}
};

}  // namespace avp

#endif /* !VIDEO_FRAME_BUFFER_H */
