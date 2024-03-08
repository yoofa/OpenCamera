/*
 * v4l2_video_source.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef V4L2_VIDEO_SOURCE_H
#define V4L2_VIDEO_SOURCE_H
#include <linux/videodev2.h>

#include "common/media_source.h"
#include "media/camera_info/video_source_factory.h"

namespace ave {
class V4L2VideoSourceForCameraInfo : public MediaSource {
 public:
  V4L2VideoSourceForCameraInfo() = delete;
  explicit V4L2VideoSourceForCameraInfo(const char* device);
  virtual ~V4L2VideoSourceForCameraInfo();

  status_t start(MetaData* params = nullptr) override;

  status_t stop() override;

  std::shared_ptr<MetaData> getFormat() override;

  status_t read(std::shared_ptr<Buffer>& buffer,
                const ReadOptions* options = nullptr) override;

  status_t pause() override;

  status_t setBuffers(const std::vector<Buffer*>&) override;

  status_t setStopTimeUs(int64_t) override;

 private:
  struct V4L2Buffer {
    uint8_t* data;
    size_t size;
  };
  int DoIoctl(int request, void* argp);
  bool MapAndQueueBuffer(int index);
  bool StartStream();
  bool StopStream();

  int fd_;
  v4l2_capability v4l2_capability_;
  std::vector<v4l2_fmtdesc> mV4L2Formats;
  v4l2_format video_format_;
  int32_t width_;
  int32_t height_;
  int32_t color_format_;
  std::vector<V4L2Buffer> buffers_;
};

std::unique_ptr<VideoSourceFactory> CreateV4l2VideoSourceFactory();

}  // namespace ave

#endif /* !V4L2_VIDEO_SOURCE_H */
