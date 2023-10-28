/*
 * audio_mixer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_mixer.h"

namespace avp {

void AudioMixer::DownMix(const int16_t* src,
                         size_t src_channels,
                         uint32_t samples_per_channel,
                         size_t dst_channels,
                         int16_t* dst) {
  //
}

void AudioMixer::UpMix(const int16_t* src,
                       size_t src_channels,
                       uint32_t samples_per_channel,
                       size_t dst_channels,
                       int16_t* dst) {
  // TODO(youfa)
}

}  // namespace avp
