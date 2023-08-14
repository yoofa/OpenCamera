/*
 * video_send_stream.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_send_stream.h"

#include <memory>

#include "api/video/video_source_interface.h"
#include "base/sequence_checker.h"
#include "media/video_stream_encoder.h"

namespace avp {

VideoSendStream::VideoSendStream(
    base::TaskRunnerFactory* task_runner_factory,
    base::TaskRunner* task_runner,
    VideoEncoderFactory* video_encoder_factory,
    VideoSourceInterface<std::shared_ptr<VideoFrame>>* video_source,
    VideoStreamSender* video_stream_sender)
    : task_runner_factory_(task_runner_factory),
      task_runner_(task_runner),
      video_encoder_factory_(video_encoder_factory),
      video_stream_sender_(video_stream_sender),
      video_source_(video_source),
      video_stream_encoder_(
          std::make_unique<VideoStreamEncoder>(task_runner_factory_,
                                               video_encoder_factory_,
                                               video_stream_sender_)) {
  video_stream_encoder_->SetSource(video_source_);
  video_stream_encoder_->SetSink(this);

  ReConfigureEncoder();
}

VideoSendStream::~VideoSendStream() {}

void VideoSendStream::Start() {
  task_runner_->PostTask([this]() { AVP_DCHECK_RUN_ON(task_runner_); });
}

void VideoSendStream::Stop() {
  task_runner_->PostTask([]() {});
}

EncodedImageCallback::Result VideoSendStream::OnEncodedImage(
    const std::shared_ptr<EncodedImage>& encoded_image) {
  video_stream_sender_->OnEncodedImage(encoded_image);
  return Result(Result::OK);
}

void VideoSendStream::ReConfigureEncoder() {
  video_stream_encoder_->ConfigureEncoder(VideoEncoderConfig(), 100);
}

}  // namespace avp
