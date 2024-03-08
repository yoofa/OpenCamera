/*
 * audio_encoder.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H

#include "api/audio/audio_frame.h"
#include "base/buffer.h"
#include "base/types.h"
#include "common/audio_codec_property.h"
#include "common/media_packet.h"

namespace ave {

class AudioEncoder {
 public:
  class EncodedCallback {
   public:
    virtual ~EncodedCallback() = default;
    virtual void OnEncoded(const MediaPacket packet) = 0;
  };

  AudioEncoder() = default;
  virtual ~AudioEncoder() = default;
  virtual status_t InitEncoder(const AudioCodecProperty& codec_settings) = 0;

  virtual status_t RegisterEncoderCompleteCallback(
      EncodedCallback* callback) = 0;

  virtual status_t Release() = 0;

  virtual status_t Encode(const std::shared_ptr<AudioFrame>& frame) = 0;

  // ongoing rate control
  // virtual void SetRate(rate);
};

}  // namespace ave

#endif /* !AUDIO_ENCODER_H */
