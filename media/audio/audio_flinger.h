/*
 * audio_flinger.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_FLINGER_H
#define AUDIO_FLINGER_H

#include "api/audio/audio_device.h"
#include "base/mutex.h"
#include "modules/audio_device/audio_device_defines.h"

namespace avp {

class AudioFlinger : public AudioTransport {
 public:
  AudioFlinger(AudioDevice* audio_device);
  ~AudioFlinger();

  bool InitIfNeed();

  status_t DataIsRecorded(const void* audio_data,
                          uint32_t bytes_per_sample,
                          uint32_t num_samples_per_channel,
                          uint32_t num_channels,
                          uint32_t sample_rate_hz) override;

  status_t GetPlaybackData(void* audio_data,
                           uint32_t bytes_per_sample,
                           uint32_t num_samples_per_channel,
                           uint32_t num_channels,
                           uint32_t sample_rate_hz) override;

 private:
  AudioDevice* audio_device_;
  bool initialized_;
  Mutex lock_;
};

}  // namespace avp

#endif /* !AUDIO_FLINGER_H */
