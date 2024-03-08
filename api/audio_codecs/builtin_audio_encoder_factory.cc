/*
 * builtin_audio_encoder_factory.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "builtin_audio_encoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "modules/audio_coding/codecs/aac/fdkaac_encoder.h"

namespace ave {

class BuiltinAudioEncoderFactory : public AudioEncoderFactory {
 public:
  virtual ~BuiltinAudioEncoderFactory() = default;
  std::unique_ptr<AudioEncoder> CreateAudioEncoder(CodecId codec_id) {
    switch (codec_id) {
      case CodecId::AV_CODEC_ID_AAC:
        return std::make_unique<FDKAACEncoder>();
      default:
        return nullptr;
    }
  }
};

std::unique_ptr<AudioEncoderFactory> CreateBuiltinAudioEncoderFactory() {
  return std::make_unique<BuiltinAudioEncoderFactory>();
}

}  // namespace ave
