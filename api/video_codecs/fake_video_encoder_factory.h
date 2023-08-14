/*
 * fake_video_encoder_factory.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef FAKE_VIDEO_ENCODER_FACTORY_H
#define FAKE_VIDEO_ENCODER_FACTORY_H

#include "api/video_codecs/video_encoder_factory.h"
namespace avp {

class FakeVideoEncoderFactory : public VideoEncoderFactory {
 public:
  std::unique_ptr<VideoEncoder> CreateVideoEncoder() override;
};

}  // namespace avp

#endif /* !FAKE_VIDEO_ENCODER_FACTORY_H */
