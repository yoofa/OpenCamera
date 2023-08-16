/*
 * video_sink_wrapper.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_sink_wrapper.h"

namespace avp {

VideoSinkWrapper::VideoSinkWrapper(
    std::shared_ptr<VideoSinkInterface<EncodedImage>> sink) {
  sink_ = std::move(sink);
}

std::shared_ptr<VideoSinkInterface<EncodedImage>> VideoSinkWrapper::Create(
    std::shared_ptr<VideoSinkInterface<EncodedImage>> sink) {
  return std::make_shared<VideoSinkWrapper>(sink);
}

}  // namespace avp
