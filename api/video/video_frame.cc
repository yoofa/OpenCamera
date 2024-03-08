/*
 * video_frame.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_frame.h"

namespace ave {
VideoFrame::VideoFrame(uint64_t id,
                       std::shared_ptr<VideoFrameBuffer> video_frame_buffer,
                       uint64_t timestamp_us,
                       std::optional<Rect> rect)
    : id_(id),
      video_frame_buffer_(video_frame_buffer),
      timestamp_us_(timestamp_us),
      rect_(rect) {}

VideoFrame::~VideoFrame() = default;

VideoFrame::VideoFrame(const VideoFrame&) = default;
VideoFrame::VideoFrame(VideoFrame&&) = default;
VideoFrame& VideoFrame::operator=(const VideoFrame&) = default;
VideoFrame& VideoFrame::operator=(VideoFrame&&) = default;

size_t VideoFrame::width() const {
  return video_frame_buffer_ ? video_frame_buffer_->width() : 0;
}

size_t VideoFrame::height() const {
  return video_frame_buffer_ ? video_frame_buffer_->height() : 0;
}

size_t VideoFrame::size() const {
  return width() * height();
}

std::shared_ptr<VideoFrameBuffer> VideoFrame::video_frame_buffer() const {
  return video_frame_buffer_;
}

void VideoFrame::set_video_frame_buffer(
    std::shared_ptr<VideoFrameBuffer> video_frame_buffer) {
  video_frame_buffer_ = std::move(video_frame_buffer);
}

}  // namespace ave
