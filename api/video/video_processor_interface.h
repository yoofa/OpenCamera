/*
 * video_processor_interface.h
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_PROCESSOR_INTERFACE_H
#define VIDEO_PROCESSOR_INTERFACE_H

#include <memory>

#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_source_interface.h"

namespace ave {

template <typename VideoFrameT>
class VideoProcessorSink {
 public:
  virtual ~VideoProcessorSink() = default;

  virtual void OnProcessedFrame(VideoFrameT& frame) = 0;
};

template <typename VideoFrameT>
class VideoProcessorInterface : public VideoSourceInterface<VideoFrameT>,
                                public VideoSinkInterface<VideoFrameT> {
 public:
  virtual ~VideoProcessorInterface() = default;

  // VideoSourceInterface
  virtual void AddOrUpdateSink(VideoSinkInterface<VideoFrameT>* sink,
                               const VideoSinkWants& wants) override = 0;

  virtual void RemoveSink(VideoSinkInterface<VideoFrameT>* sink) override = 0;

  // VideoSInkInterface
  virtual void OnFrame(const VideoFrameT& frame) override = 0;

  // processing frame
  virtual void OnProcess(const VideoFrameT& frame) = 0;

  // frame processed
  void OnProcessed(VideoFrameT frame) {
    if (video_sink_ != nullptr) {
      video_sink_->OnFrame(frame);
    }
  }

 private:
  VideoSourceInterface<VideoFrameT>* video_source_;
  // video_sink_ lifecycle longger than processor
  VideoSinkInterface<VideoFrameT>* video_sink_;
};

}  // namespace ave

#endif /* !VIDEO_PROCESSOR_INTERFACE_H */
