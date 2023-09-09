/*
 * audio_flinger.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_flinger.h"
#include "api/audio/audio_device.h"
#include "base/logging.h"

namespace {
// constexpr uint16_t kAudioDeviceId = 0;
}

namespace avp {

AudioFlinger::AudioFlinger(AudioDevice* audio_device)
    : audio_device_(audio_device), initialized_(false) {}

AudioFlinger::~AudioFlinger() {}

bool AudioFlinger::InitIfNeed() {
  lock_guard l(lock_);
  if (initialized_) {
    return true;
  }

  if (audio_device_->Init() != 0) {
    LOG(LS_ERROR) << "AudioDevice Init failed";
  }

  if (audio_device_->SetRecordingDevice(4) != 0) {
    LOG(LS_ERROR) << "AudioDevice SetRecordingDevice failed";
  }

  if (audio_device_->InitMicrophone() != 0) {
    LOG(LS_ERROR) << "AudioDevice InitMicrophone failed";
  }

  if (audio_device_->RegisterAudioCallback(this) != 0) {
    LOG(LS_ERROR) << "AudioDevice RegisterAudioCallback failed";
  }

  // TODO(youfa) move to audio stream sender create time
  if (audio_device_->InitRecording() != 0) {
    LOG(LS_ERROR) << "AudioDevice InitRecording failed";
  }

  if (audio_device_->StartRecording() != 0) {
    LOG(LS_ERROR) << "AudioDevice StartRecording failed";
  }

  return true;
}

status_t AudioFlinger::DataIsRecorded(const void* audio_data,
                                      uint32_t bytes_per_sample,
                                      uint32_t num_samples_per_channel,
                                      uint32_t num_channels,
                                      uint32_t sample_rate_hz) {
  LOG(LS_INFO) << "AudioFlinger::DataIsRecorded";
  // remix and resample

  // send to audio stream sender
  return OK;
}

status_t AudioFlinger::GetPlaybackData(void* audio_data,
                                       uint32_t bytes_per_sample,
                                       uint32_t num_samples_per_channel,
                                       uint32_t num_channels,
                                       uint32_t sample_rate_hz) {
  return OK;
}

}  // namespace avp
