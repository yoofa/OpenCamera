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
#include "media/video_source_factory.h"

namespace avp {
class V4L2VideoSource : public MediaSource {
 public:
  V4L2VideoSource() = delete;
  explicit V4L2VideoSource(const char* device);
  virtual ~V4L2VideoSource();

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
  int doIoctl(int request, void* argp);
  bool mapAndQueueBuffer(int index);
  bool startStream();
  bool stopStream();

  int mFd;
  v4l2_capability mV4L2Capability;
  std::vector<v4l2_fmtdesc> mV4L2Formats;
  v4l2_format mVideoFmt;
  int32_t mWidth;
  int32_t mHeight;
  int32_t mColorFormat;
  std::vector<V4L2Buffer> mBuffers;
};

std::unique_ptr<VideoSourceFactory> CreateV4l2VideoSourceFactory();

}  // namespace avp

#endif /* !V4L2_VIDEO_SOURCE_H */
