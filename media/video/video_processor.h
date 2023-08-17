/*
 * video_processor.h
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_PROCESSOR_H
#define VIDEO_PROCESSOR_H

#include <memory>

#include "common/buffer.h"
#include "media/video_sink.h"

namespace avp {
class VideoProcessor {
 public:
  virtual ~VideoProcessor() = default;

  // processing frame
  virtual void OnProcess(std::shared_ptr<Buffer> frame) = 0;

  // set the video sink of processor
  void SetVideoSink(VideoSink* sink) { video_sink_ = sink; }

  // frame processed
  void OnProcessed(std::shared_ptr<Buffer> frame) {
    if (video_sink_ != nullptr) {
      video_sink_->OnFrame(frame);
    }
  }

 private:
  // video_sink_ lifecycle longger than processor
  VideoSink* video_sink_;
};

}  // namespace avp

#endif /* !VIDEO_PROCESSOR_H */
