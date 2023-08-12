/*
 * v4l2_video_source.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef V4L2_VIDEO_SOURCE_H
#define V4L2_VIDEO_SOURCE_H
#include <linux/videodev2.h>

#include "api/video/video_frame.h"
#include "base/task_util/repeating_task.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "common/message.h"
#include "common/meta_data.h"
#include "media/video_source_base.h"

namespace avp {
class V4L2VideoSource : public VideoSourceBase<std::shared_ptr<VideoFrame>> {
 protected:
  // for private construct
  struct protect_parameter {
    explicit protect_parameter() {}
  };

 public:
  static std::shared_ptr<V4L2VideoSource> Create(std::shared_ptr<Message> info);

  explicit V4L2VideoSource(const char* device,
                           protect_parameter = protect_parameter());
  virtual ~V4L2VideoSource() override;

  // VideoSourceBase implementation.
  void AddOrUpdateSink(VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink,
                       const VideoSinkWants& wants) override;
  void RemoveSink(
      VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink) override;

  status_t start(MetaData* params = nullptr);

  status_t stop();

  status_t read(std::shared_ptr<VideoFrame>& buffer);

  status_t pause();

 private:
  struct V4L2Buffer {
    uint8_t* data;
    size_t size;
  };
  int doIoctl(int request, void* argp);
  bool mapAndQueueBuffer(int index);
  bool startStream();
  bool stopStream();

  std::unique_ptr<base::TaskRunnerFactory> task_runner_factory_;
  std::unique_ptr<base::TaskRunner> task_runner_;
  RepeatingTaskHandle repeating_task_handler_;

  int mFd;
  v4l2_capability mV4L2Capability;
  std::vector<v4l2_fmtdesc> mV4L2Formats;
  v4l2_format mVideoFmt;
  int32_t mWidth;
  int32_t mHeight;
  int32_t mColorFormat;
  std::vector<V4L2Buffer> mBuffers;
  uint64_t frame_count_;
};

// std::unique_ptr<VideoSourceFactory> CreateV4l2VideoSourceFactory();

}  // namespace avp

#endif /* !V4L2_VIDEO_SOURCE_H */
