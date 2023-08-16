/*
 * builtin_video_encoder_factory.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "builtin_video_encoder_factory.h"

#include "api/video_codecs/video_encoder.h"
#include "common/video_codec_property.h"
#include "modules/video_coding/codecs/h264/openh264_encoder.h"

namespace avp {

class BuiltinVideoEncoder : public VideoEncoder {
 public:
  BuiltinVideoEncoder() = default;
  ~BuiltinVideoEncoder() override = default;

  status_t InitEncoder(const VideoCodecProperty& codec_settings,
                       const Settings& settings) override {
    return OK;
  }

  status_t Release() override { return OK; }

  status_t Encode(const std::shared_ptr<VideoFrame>& frame) override {
    return OK;
  }

  status_t RegisterEncoderCompleteCallback(
      EncodedImageCallback* callback) override {
    return OK;
  }

  void RequestKeyFrame() override {}
};

class BuiltinVideoEncoderFactory : public VideoEncoderFactory {
 public:
  virtual ~BuiltinVideoEncoderFactory() override = default;

  std::unique_ptr<VideoEncoder> CreateVideoEncoder() override {
    return std::make_unique<OpenH264Encoder>();
  }
};

std::unique_ptr<VideoEncoderFactory> CreateBuiltinVideoEncoderFactory() {
  return std::make_unique<BuiltinVideoEncoderFactory>();
}
}  // namespace avp
