/*
 * video_sink_wrapper.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_SINK_WRAPPER_H
#define VIDEO_SINK_WRAPPER_H

#include <memory>

#include "api/video/encoded_image.h"
#include "api/video/video_sink_interface.h"
#include "common/message.h"

namespace avp {
class VideoSinkWrapper : public VideoSinkInterface<EncodedImage>,
                         public MessageObject {
 public:
  static std::shared_ptr<VideoSinkInterface<EncodedImage>> Create(
      std::shared_ptr<VideoSinkInterface<EncodedImage>> sink);

  void OnFrame(const EncodedImage& frame) override { sink_->OnFrame(frame); }

  explicit VideoSinkWrapper(
      std::shared_ptr<VideoSinkInterface<EncodedImage>> sink);

 private:
  std::shared_ptr<VideoSinkInterface<EncodedImage>> sink_;
};

}  // namespace avp

#endif /* !VIDEO_SINK_WRAPPER_H */
