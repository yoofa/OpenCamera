/*
 * video_capturer.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_CAPTURER_H
#define VIDEO_CAPTURER_H

#include <cstdint>
#include <memory>
#include <mutex>

#include "api/video/video_frame.h"
#include "api/video/video_processor_interface.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_source_interface.h"
#include "base/task_util/repeating_task.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/types.h"
#include "common/message.h"
#include "media/video_source_base.h"

namespace avp {

class VideoCapturer : public VideoSinkInterface<std::shared_ptr<VideoFrame>>,
                      public VideoProcessorSink<std::shared_ptr<VideoFrame>>,
                      public VideoSourceInterface<std::shared_ptr<VideoFrame>>,
                      public MessageObject {
  using VideoSource = VideoSourceInterface<std::shared_ptr<VideoFrame>>;
  using VideoSink = VideoSinkInterface<std::shared_ptr<VideoFrame>>;
  using VideoProcessor = VideoProcessorInterface<std::shared_ptr<VideoFrame>>;

 public:
  explicit VideoCapturer(std::shared_ptr<Message> capture_info);
  virtual ~VideoCapturer();

  // VideoSourceInterface implementation
  void AddOrUpdateSink(VideoSink* sink, const VideoSinkWants& wants) override;
  void RemoveSink(VideoSink* sink) override;

  // VideoSinkInterface implementation
  void OnFrame(const std::shared_ptr<VideoFrame>& frame) override;

  // VideoProcessorSink implementation
  void OnProcessedFrame(std::shared_ptr<VideoFrame>& frame) override;

  void SetVideoSource(VideoSource* video_source, const VideoSinkWants& wants);

  // support only one processor, if need multiple processor, combine into one
  // processor
  status_t SetProcessor(std::shared_ptr<VideoProcessor>& processor);

  uint64_t frame_received() const { return frame_received_; }
  uint64_t frame_sent() const { return frame_sent_; }

 private:
  // with lock
  void AddOrUpdateSinkInternal(VideoSink* sink, const VideoSinkWants& wants);

  std::shared_ptr<Message> capture_info_;

  // capture runner, used to handle capture task
  std::unique_ptr<base::TaskRunnerFactory> task_runner_factory_;
  std::unique_ptr<base::TaskRunner> task_runner_;
  RepeatingTaskHandle repeating_task_handler_;

  VideoSource* video_source_;

  // std::shared_ptr<VideoProcessor> video_processor_;
  std::shared_ptr<VideoProcessor> video_processor_;

  // sinks broadcaster
  VideoSourceBase<std::shared_ptr<VideoFrame>> sinks_broadcaster_;

  uint64_t frame_received_;
  uint64_t frame_sent_;
};

}  // namespace avp

#endif /* !VIDEO_CAPTURER_H */
