/*
 * file_sink.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef FILE_SINK_H
#define FILE_SINK_H
#include <fstream>
#include <memory>
#include <type_traits>

#include "api/video/encoded_image.h"
#include "api/video/video_frame.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_sink_interface.h"
#include "api/video/yuyv_buffer.h"
#include "base/logging.h"
#include "common/buffer.h"
#include "common/message.h"

namespace ave {

template <typename T>
class FileSink : public VideoSinkInterface<T>, public MessageObject {
 public:
  explicit FileSink(const char* file) { file_.open(file); }
  ~FileSink() { file_.close(); }

  void OnFrameInternal(const std::shared_ptr<VideoFrame>& frame) {
    auto buffer = frame->video_frame_buffer();
    bool i420_option = false;
    if (i420_option &&
        buffer->pixel_format() == VideoFrameBuffer::PixelFormat::kYUY2) {
      std::shared_ptr<YUYVBufferInterface> frame_buffer =
          std::dynamic_pointer_cast<YUYVBufferInterface>(buffer);
      AVE_LOG(LS_INFO) << "YUY2: " << frame_buffer->Stride()
                       << frame_buffer->height();
      file_.write((char*)frame_buffer->Data(),
                  frame_buffer->Stride() * frame_buffer->height());
      // std::shared_ptr<YUYVBufferInterface> yuyv_buffer =
      //    YUYVBuffer::Create(640, 480);
      // AVE_LOG(LS_INFO) << "yuyv_buffer: " << yuyv_buffer->Stride();

    } else {
      auto i420_buffer = buffer->ToI420();
      file_.write((char*)i420_buffer->DataY(),
                  i420_buffer->StrideY() * i420_buffer->height());
      file_.write((char*)i420_buffer->DataU(),
                  i420_buffer->StrideU() * i420_buffer->height() / 2);
      file_.write((char*)i420_buffer->DataV(),
                  i420_buffer->StrideV() * i420_buffer->height() / 2);
    }
  }
  void OnFrameInternal(const EncodedImage& frame) {
    // AVE_LOG(LS_INFO) << "EncodedImage, size: " << frame.Size();
    file_.write((char*)frame.Data(), frame.Size());
  }

  void OnFrame(const T& frame) override {
    static_assert(std::is_same_v<T, std::shared_ptr<VideoFrame>> ||
                      std::is_same_v<T, EncodedImage>,
                  "Unsupported frame type.");

    if (!file_.is_open()) {
      std::cerr << "File is not open!" << std::endl;
      return;
    }

    if constexpr (std::is_same_v<T, std::shared_ptr<VideoFrame>>) {
      OnFrameInternal(frame);
    } else if constexpr (std::is_same_v<T, EncodedImage>) {
      OnFrameInternal(frame);
    }
  }

 private:
  std::ofstream file_;
};

}  // namespace ave

#endif /* !FILE_SINK_H */
