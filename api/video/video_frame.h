/*
 * video_frame.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_FRAME_H
#define VIDEO_FRAME_H

#include <memory>
#include <optional>

#include "base/checks.h"
#include "base/types.h"

#include "api/video/video_frame_buffer.h"

namespace ave {

class VideoFrame {
 public:
  struct Rect {
    // left
    size_t offset_x;
    // top
    size_t offset_y;
    size_t width;
    size_t height;

    // check each member
    bool operator==(const Rect& other) const {
      return offset_x == other.offset_x && offset_y == other.offset_y &&
             width == other.width && height == other.height;
    }

    bool operator!=(const Rect& other) const { return !(*this == other); }
  };

  VideoFrame(uint64_t id,
             std::shared_ptr<VideoFrameBuffer> video_frame_buffer,
             uint64_t timestamp_us,
             std::optional<Rect> rect);
  VideoFrame(VideoFrame&&);
  VideoFrame(const VideoFrame&);
  VideoFrame& operator=(VideoFrame&&);
  VideoFrame& operator=(const VideoFrame&);
  ~VideoFrame();

  // get frame width
  size_t width() const;
  // get frame height
  size_t height() const;
  // get frame size in pixels
  size_t size() const;

  // id, frame sequance from the origin video source.
  uint64_t id() const { return id_; }
  void set_id(uint64_t id) { id_ = id; }

  // video frame buffer
  std::shared_ptr<VideoFrameBuffer> video_frame_buffer() const;
  void set_video_frame_buffer(
      std::shared_ptr<VideoFrameBuffer> video_frame_buffer);

  // system monotonic clock timestamp in microseconds
  uint64_t timestamp_us() const { return timestamp_us_; }
  void set_timestamp_us(uint64_t timestamp_us) { timestamp_us_ = timestamp_us; }

  // crop rect
  const std::optional<Rect> rect() const {
    return rect_.value_or(Rect{0, 0, width(), height()});
  }
  void set_rect(std::optional<Rect>& rect) {
    AVE_DCHECK_GE(rect->offset_x, 0);
    AVE_DCHECK_GE(rect->offset_y, 0);
    AVE_DCHECK_LE(rect->offset_x + rect->width, width());
    AVE_DCHECK_LE(rect->offset_y + rect->height, height());

    rect_ = rect;
  }

 private:
  uint64_t id_;
  std::shared_ptr<VideoFrameBuffer> video_frame_buffer_;
  uint64_t timestamp_us_;
  std::optional<Rect> rect_;
};
}  // namespace ave

#endif /* !VIDEO_FRAME_H */
