/*
 * video_source_factory.h
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_SOURCE_FACTORY_H
#define VIDEO_SOURCE_FACTORY_H

#include <memory>

#include "common/media_source.h"
#include "common/message.h"

namespace avp {
class VideoSourceFactory {
 public:
  virtual ~VideoSourceFactory() = default;

  // video_info contains some infomation which used to create video source in
  // different platform, ex:
  // "/dev/video0" in linux v4l2 camera
  virtual std::unique_ptr<MediaSource> CreateVideoSource(
      std::shared_ptr<Message> video_info) = 0;
};

}  // namespace avp

#endif /* !VIDEO_SOURCE_FACTORY_H */
