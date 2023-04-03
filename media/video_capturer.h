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

#include "base/task_util/repeating_task.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/types.h"
#include "common/handler.h"
#include "common/looper.h"
#include "common/media_source.h"
#include "media/video_processor.h"
#include "media/video_sink.h"
#include "media/video_source_factory.h"

namespace avp {
class VideoCapturer;

class VideoCapturer : public Handler, VideoSink {
  //                      std::enable_shared_from_this<VideoSink> {
 public:
  explicit VideoCapturer(std::shared_ptr<Looper> looper,
                         std::shared_ptr<Message> capture_info);
  virtual ~VideoCapturer();

  status_t Init();
  status_t Start();
  status_t Stop();

  status_t AddVideoSink(std::shared_ptr<VideoSink> sink);

  // support only one processor, if need multiple processor, combine into one
  // processor
  status_t SetProcessor(std::shared_ptr<VideoProcessor> processor);

  // when frame captured, notify all sinks or processor
  void OnFrameCaptured(std::shared_ptr<Buffer> buffer);

  // capture one frame, return capture interval
  uint64_t CaptureOneFrame();

  // observer like motion detector will notify motion event which can control
  // capturer framerate.
  void OnSinkNotify(std::shared_ptr<Message> notify);

  // video sink callback
  void OnFrame(std::shared_ptr<Buffer> buffer) override;

  enum {
    kWhatSinkNotify = 'snfy',
  };

 private:
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  std::shared_ptr<Message> capture_info_;
  std::unique_ptr<VideoSourceFactory> video_source_factory_;
  std::unique_ptr<MediaSource> video_source_;

  // message looper, used to handle sink notify message
  std::shared_ptr<Looper> looper_;

  // capture runner, used to handle capture task
  std::unique_ptr<base::TaskRunnerFactory> task_runner_factory_;
  std::unique_ptr<base::TaskRunner> task_runner_;
  RepeatingTaskHandle repeating_task_handler_;

  // sinks
  std::vector<std::shared_ptr<VideoSink>> video_sinks_;

  std::shared_ptr<VideoProcessor> video_processor_;

  std::mutex processor_lock_;
  std::mutex sink_lock_;
};

}  // namespace avp

#endif /* !VIDEO_CAPTURER_H */
