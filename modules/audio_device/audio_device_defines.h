/*
 * audio_device_defines.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_DEVICE_DEFINES_H
#define AUDIO_DEVICE_DEFINES_H

#include <stddef.h>

#include <string>

#include "base/checks.h"
#include "base/types.h"

namespace ave {

static const int kAdmMaxDeviceNameSize = 128;
static const int kAdmMaxFileNameSize = 512;
static const int kAdmMaxGuidSize = 128;

static const int kAdmMinPlayoutBufferSizeMs = 10;
static const int kAdmMaxPlayoutBufferSizeMs = 250;

// ----------------------------------------------------------------------------
//  AudioTransport
// ----------------------------------------------------------------------------

class AudioTransport {
 public:
  virtual status_t DataIsRecorded(const void* audio_data,
                                  uint32_t bytes_per_sample,
                                  uint32_t num_samples_per_channel,
                                  uint32_t num_channels,
                                  uint32_t sample_rate_hz) = 0;

  virtual status_t GetPlaybackData(void* audio_data,
                                   uint32_t bytes_per_sample,
                                   uint32_t num_samples_per_channel,
                                   uint32_t num_channels,
                                   uint32_t sample_rate_hz) = 0;

 protected:
  virtual ~AudioTransport() {}
};
}  // namespace ave

#endif /* !AUDIO_DEVICE_DEFINES_H */
