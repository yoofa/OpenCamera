/*
 * audio_encoder_factory.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_ENCODER_FACTORY_H
#define AUDIO_ENCODER_FACTORY_H

#include <memory>

#include "api/audio_codecs/audio_encoder.h"
#include "common/codec_id.h"

namespace avp {
class AudioEncoderFactory {
 public:
  virtual ~AudioEncoderFactory() = default;

  virtual std::unique_ptr<AudioEncoder> CreateAudioEncoder(
      CodecId codec_id) = 0;
};
}  // namespace avp

#endif /* !AUDIO_ENCODER_FACTORY_H */
