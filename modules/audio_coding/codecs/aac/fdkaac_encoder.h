/*
 * fdkaac_encoder.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef FDKAAC_ENCODER_H
#define FDKAAC_ENCODER_H

#include "api/audio_codecs/audio_encoder.h"
#include "third_party/fdk-aac/src/libAACenc/include/aacenc_lib.h"

namespace avp {

class FDKAACEncoder : public AudioEncoder {
 public:
  FDKAACEncoder();
  virtual ~FDKAACEncoder();

  virtual status_t InitEncoder(
      const AudioCodecProperty& codec_settings) override;

  virtual status_t RegisterEncoderCompleteCallback(
      EncodedCallback* callback) override;

  virtual status_t Release() override;

  virtual status_t Encode(const std::shared_ptr<AudioFrame>& frame) override;

 private:
  EncodedCallback* callback_;
  HANDLE_AACENCODER encoder_;
};

}  // namespace avp
#endif /* !FDKAAC_ENCODER_H */
