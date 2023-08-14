/*
 * video_stream_sender.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_stream_sender.h"

#include "api/video_codecs/video_encoder.h"
#include "base/checks.h"
#include "base/logging.h"

namespace avp {

VideoStreamSender::VideoStreamSender(base::TaskRunner* transport_runner)
    : transport_runner_(transport_runner) {
  // TODO(youfa) transport_runner_ not used, check to ignore build warning
  DCHECK(transport_runner_);
}
VideoStreamSender::~VideoStreamSender() {}

bool VideoStreamSender::frame_wanted() const {
  lock_guard guard(sink_lock_);
  return broadcaster_.frame_wanted();
}

EncodedImageCallback::Result VideoStreamSender::OnEncodedImage(
    const std::shared_ptr<EncodedImage>& encoded_image) {
  lock_guard guard(sink_lock_);

  broadcaster_.OnFrame(encoded_image);
  return Result(Result::OK);
}

void VideoStreamSender::AddVideoSink(
    const std::shared_ptr<VideoSinkInterface<std::shared_ptr<EncodedImage>>>&
        sink) {
  lock_guard guard(sink_lock_);
  // add reference to sinks_
  sinks_.push_back(sink);
  broadcaster_.AddOrUpdateSink(sink.get(), VideoSinkWants());
}

void VideoStreamSender::RemoveVideoSink(
    const std::shared_ptr<VideoSinkInterface<std::shared_ptr<EncodedImage>>>&
        sink) {
  lock_guard guard(sink_lock_);
  // remove reference from sinks_
  broadcaster_.RemoveSink(sink.get());
  sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink), sinks_.end());
}

}  // namespace avp
