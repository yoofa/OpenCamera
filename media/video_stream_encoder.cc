/*
 * video_stream_encoder.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_stream_encoder.h"

#include "api/video/encoded_image.h"
#include "base/logging.h"
#include "base/sequence_checker.h"
#include "base/task_util/task_runner_base.h"
#include "common/video_codec_property.h"

namespace avp {

VideoStreamEncoder::VideoStreamEncoder(
    base::TaskRunnerFactory* task_runner_factory,
    VideoEncoderFactory* encoder_factory,
    EncodedImageCallback* sink)
    : task_runner_factory_(task_runner_factory),
      encoder_factory_(encoder_factory),
      sink_(sink),
      pending_encoder_reconfiguration_(false),
      pending_encoder_creation_(false),
      max_data_payload_length_(0),
      encoder_runner_(task_runner_factory_->CreateTaskRunner(
          "VideoStreamEncoder",
          base::TaskRunnerFactory::Priority::NORMAL)) {
  // TODO(youfa) check encoder_factory to ignore build warning
  DCHECK(encoder_factory_);
}

VideoStreamEncoder::~VideoStreamEncoder() {}

void VideoStreamEncoder::SetSource(
    VideoSourceInterface<std::shared_ptr<VideoFrame>>* source) {
  DCHECK(source != nullptr);
  source->AddOrUpdateSink(this, VideoSinkWants());
}

void VideoStreamEncoder::SetSink(EncoderSink* sink) {}

void VideoStreamEncoder::SetStartBitrate(int start_bitrate_bps) {}

void VideoStreamEncoder::SendKeyFrame() {}

void VideoStreamEncoder::ConfigureEncoder(VideoEncoderConfig config,
                                          size_t max_data_payload_length) {
  encoder_runner_.PostTask(
      [this, config = std::move(config), max_data_payload_length]() mutable {
        AVP_DCHECK_RUN_ON(&encoder_runner_);
        LOG(LS_INFO) << "request configureEncoder";
        pending_encoder_creation_ =
            (!encoder_ || max_data_payload_length_ != max_data_payload_length);
        encoder_config_ = std::move(config);

        max_data_payload_length_ = max_data_payload_length;
        pending_encoder_reconfiguration_ = true;

        if (last_frame_info_) {
          // ReconfigureEncoder();
        } else {
          // TODO(youfa) check encoder if has internal source,
          ReConfigureEncoder();
        }
      });
}

void VideoStreamEncoder::OnFrame(const std::shared_ptr<VideoFrame>& frame) {
  encoder_runner_.PostTask([this, frame]() {
    // LOG(LS_INFO) << "VideoStreamEncoder::OnFrame";
    MaybeEncodeVideoFrame(frame);
  });
}

void VideoStreamEncoder::MaybeEncodeVideoFrame(
    const std::shared_ptr<VideoFrame>& frame) {
  AVP_DCHECK_RUN_ON(&encoder_runner_);

  // check if encoder need reconfigure
  if (!last_frame_info_ || last_frame_info_->width != frame->width() ||
      last_frame_info_->height != frame->height() ||
      last_frame_info_->type != frame->video_frame_buffer()->type()) {
    pending_encoder_reconfiguration_ = true;
    last_frame_info_ = VideoFrameInfo(frame->width(), frame->height(),
                                      frame->video_frame_buffer()->type());
    LOG(LS_INFO) << "video frame info changed, "
                 << VideoFrameBuffer::TypeToString(
                        frame->video_frame_buffer()->type())
                 << " : [" << last_frame_info_->width << ", "
                 << last_frame_info_->height << "]";
  }

  if (pending_encoder_reconfiguration_) {
    ReConfigureEncoder();
  }
  EncodeVideoFrame(frame);
}

void VideoStreamEncoder::EncodeVideoFrame(
    const std::shared_ptr<VideoFrame>& frame) {
  AVP_DCHECK_RUN_ON(&encoder_runner_);
  if (!encoder_) {
    LOG(LS_ERROR) << "Encoder not created";
    return;
  }
  status_t ret = encoder_->Encode(frame);
  if (ret < 0) {
    LOG(LS_ERROR) << "Encode failed: " << ret;
  }
}

void VideoStreamEncoder::ReConfigureEncoder() {
  DCHECK(pending_encoder_reconfiguration_);

  if (pending_encoder_creation_) {
    encoder_.reset();

    LOG(LS_INFO) << "create encoder";
    encoder_ = encoder_factory_->CreateVideoEncoder();
    LOG(LS_INFO) << "encoder created " << encoder_.get();
    CHECK(encoder_);

    pending_encoder_creation_ = false;
    pending_encoder_reconfiguration_ = false;

    encoder_->InitEncoder(VideoCodecProperty(),
                          Settings(Capabilities(false), 0, 10));

    encoder_->RegisterEncodedImageCallback(sink_);

    return;
  }
}

void VideoStreamEncoder::Stop() {}

}  // namespace avp
