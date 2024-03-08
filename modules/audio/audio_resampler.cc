/*
 * audio_resampler.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_resampler.h"

namespace ave {

void AudioResampler::Resample_s16(const int16_t* src,
                                  uint32_t src_sample_rate,
                                  size_t src_size,
                                  size_t channels,
                                  int16_t* dst,
                                  size_t dst_size,
                                  uint32_t dst_sample_rate) {
  //
}

}  // namespace ave
