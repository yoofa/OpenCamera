/*
 * audio_mixer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include "base/types.h"

namespace avp {

class AudioMixer {
 public:
  static void DownMix(const int16_t* src,
                      size_t src_channels,
                      uint32_t samples_per_channel,
                      size_t dst_channels,
                      int16_t* dst);

  static void UpMix(const int16_t* src,
                    size_t src_channels,
                    uint32_t samples_per_channel,
                    size_t dst_channels,
                    int16_t* dst);
};

}  // namespace avp

#endif /* !AUDIO_MIXER_H */
