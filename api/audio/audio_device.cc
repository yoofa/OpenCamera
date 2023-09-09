/*
 * audio_device.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_device.h"

#include "base/checks.h"
#include "media/audio/audio_callback.h"

namespace avp {

status_t AudioDevice::RegisterAudioCallback(AudioCallback* callback) {
  callback_ = callback;
  return OK;
}

status_t AudioDevice::DataIsRecorded(const void* audio_data,
                                     uint32_t num_samples,
                                     uint32_t bytes_per_sample,
                                     uint32_t num_channels,
                                     uint32_t sample_rate_hz) {
  if (callback_ != nullptr) {
    return callback_->DataIsRecorded(audio_data, num_samples, bytes_per_sample,
                                     num_channels, sample_rate_hz);
  }
  return NO_INIT;
}

status_t AudioDevice::GetPlaybackData(void* audio_data,
                                      uint32_t num_samples,
                                      uint32_t bytes_per_sample,
                                      uint32_t num_channels,
                                      uint32_t sample_rate_hz) {
  if (callback_ != nullptr) {
    return callback_->GetPlaybackData(audio_data, num_samples, bytes_per_sample,
                                      num_channels, sample_rate_hz);
  }
  return NO_INIT;
}

}  // namespace avp
