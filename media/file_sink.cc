/*
 * file_sink.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "file_sink.h"

#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/yuyv_buffer.h"
#include "base/logging.h"

namespace avp {

void FileSink::OnFrame(const std::shared_ptr<VideoFrame>& frame) {
  auto buffer = frame->video_frame_buffer();
  bool i420_option = false;
  if (i420_option &&
      buffer->pixel_format() == VideoFrameBuffer::PixelFormat::kYUY2) {
    std::shared_ptr<YUYVBufferInterface> frame_buffer =
        std::dynamic_pointer_cast<YUYVBufferInterface>(buffer);
    LOG(LS_INFO) << "YUY2: " << frame_buffer->Stride()
                 << frame_buffer->height();
    file_.write((char*)frame_buffer->Data(),
                frame_buffer->Stride() * frame_buffer->height());
    // std::shared_ptr<YUYVBufferInterface> yuyv_buffer =
    //    YUYVBuffer::Create(640, 480);
    // LOG(LS_INFO) << "yuyv_buffer: " << yuyv_buffer->Stride();

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

}  // namespace avp
