/*
 * fake_video_encoder_factory.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "fake_video_encoder_factory.h"
#include <memory>

#include "api/video/encoded_image.h"
#include "api/video_codecs/video_encoder.h"
#include "base/logging.h"
#include "base/types.h"
#include "common/video_codec_property.h"

namespace ave {

class FakeVideoEncoder : public VideoEncoder {
  status_t InitEncoder(const VideoCodecProperty& codec_settings,
                       const Settings& settings) override {
    return OK;
  }

  status_t RegisterEncoderCompleteCallback(
      EncodedImageCallback* callback) override {
    callback_ = callback;
    return OK;
  }

  status_t Release() override { return OK; }

  status_t Encode(const std::shared_ptr<VideoFrame>& frame) override {
    auto i420_buffer = frame->video_frame_buffer()->ToI420();
    size_t size = i420_buffer->StrideY() * i420_buffer->height() * 3 / 2;

    auto encoded_data = EncodedImageBuffer::Create(size);
    memcpy(encoded_data->Data(), i420_buffer->DataY(),
           i420_buffer->StrideY() * i420_buffer->height());
    memcpy(
        encoded_data->Data() + i420_buffer->StrideY() * i420_buffer->height(),
        i420_buffer->DataU(),
        i420_buffer->StrideU() * i420_buffer->height() / 2);
    memcpy(encoded_data->Data() +
               i420_buffer->StrideY() * i420_buffer->height() +
               i420_buffer->StrideU() * i420_buffer->height() / 2,
           i420_buffer->DataV(),
           i420_buffer->StrideV() * i420_buffer->height() / 2);

    // auto encoded_image = std::make_shared<EncodedImage>();

    auto encoded_image = EncodedImage();
    encoded_image.SetEncodedData(encoded_data);
    encoded_image.encoded_width_ = frame->width();
    encoded_image.encoded_height_ = frame->height();
    encoded_image.SetTimestamp(frame->timestamp_us());
    callback_->OnEncodedImage(encoded_image);
    return OK;
  }

  void RequestKeyFrame() override {}

 private:
  EncodedImageCallback* callback_;
};

std::unique_ptr<VideoEncoder> FakeVideoEncoderFactory::CreateVideoEncoder() {
  return std::make_unique<FakeVideoEncoder>();
}

}  // namespace ave
