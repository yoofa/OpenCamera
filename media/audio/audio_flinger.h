/*
 * audio_flinger.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_FLINGER_H
#define AUDIO_FLINGER_H

#include "api/audio/audio_device.h"
#include "api/audio/audio_frame.h"
#include "base/mutex.h"
#include "base/thread_annotation.h"
#include "media/audio/audio_send_stream.h"
#include "modules/audio_device/audio_device_defines.h"

namespace ave {

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

  void UpdateSender(std::vector<std::shared_ptr<AudioSendStream>> senders);

 private:
  void SendFrame(std::unique_ptr<AudioFrame> audio_frame);

  AudioDevice* audio_device_;
  bool initialized_;

  Mutex sender_lock_;
  std::vector<std::shared_ptr<AudioSendStream>> senders_
      GUARDED_BY(sender_lock_);
  uint32_t sample_rate_hz_;
  size_t num_channels_;
};

}  // namespace ave

#endif /* !AUDIO_FLINGER_H */
