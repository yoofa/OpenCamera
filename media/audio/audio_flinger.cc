/*
 * audio_flinger.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_flinger.h"

#include <memory>

#include "api/audio/audio_device.h"
#include "base/logging.h"
#include "media/audio/remix_resample.h"

namespace {
// constexpr uint16_t kAudioDeviceId = 0;
}

namespace ave {
namespace {
// We want to process at the lowest sample rate and channel count possible
// without losing information. Choose the lowest native rate at least equal to
// the minimum of input and codec rates, choose lowest channel count, and
// configure the audio frame.
// void InitializeCaptureFrame(int input_sample_rate,
//                            int send_sample_rate_hz,
//                            size_t input_num_channels,
//                            size_t send_num_channels,
//                            AudioFrame* audio_frame) {
//  AVE_DCHECK(audio_frame);
//  int min_processing_rate_hz = std::min(input_sample_rate,
//  send_sample_rate_hz); audio_frame->sample_rate_hz_ = min_processing_rate_hz;
//  audio_frame->num_channels_ = std::min(input_num_channels,
//  send_num_channels);
//}

}  // namespace

AudioFlinger::AudioFlinger(AudioDevice* audio_device)
    : audio_device_(audio_device), initialized_(false) {}

AudioFlinger::~AudioFlinger() {}

bool AudioFlinger::InitIfNeed() {
  lock_guard l(&sender_lock_);
  if (initialized_) {
    return true;
  }

  if (audio_device_->Init() != 0) {
    AVE_LOG(LS_ERROR) << "AudioDevice Init failed";
  }

  if (audio_device_->SetRecordingDevice(4) != 0) {
    AVE_LOG(LS_ERROR) << "AudioDevice SetRecordingDevice failed";
  }

  if (audio_device_->InitMicrophone() != 0) {
    AVE_LOG(LS_ERROR) << "AudioDevice InitMicrophone failed";
  }

  if (audio_device_->RegisterAudioCallback(this) != 0) {
    AVE_LOG(LS_ERROR) << "AudioDevice RegisterAudioCallback failed";
  }

  // TODO(youfa) move to audio stream sender create time
  if (audio_device_->InitRecording() != 0) {
    AVE_LOG(LS_ERROR) << "AudioDevice InitRecording failed";
  }

  if (audio_device_->StartRecording() != 0) {
    AVE_LOG(LS_ERROR) << "AudioDevice StartRecording failed";
  }

  return true;
}

status_t AudioFlinger::DataIsRecorded(const void* audio_data,
                                      uint32_t bytes_per_sample,
                                      uint32_t samples_per_channel,
                                      uint32_t num_channels,
                                      uint32_t sample_rate_hz) {
  AVE_LOG(LS_VERBOSE) << "AudioFlinger::DataIsRecorded";
  // remix and resample
  uint32_t send_sample_rate_hz = 0;
  size_t send_num_channels = 0;

  {
    lock_guard l(&sender_lock_);
    send_sample_rate_hz = sample_rate_hz_;
    send_num_channels = num_channels_;
  }

  auto audio_frame = std::make_unique<AudioFrame>();

  RemixAndResample((const int16_t*)audio_data, num_channels, sample_rate_hz,
                   samples_per_channel, audio_frame->mutable_data(),
                   send_num_channels, send_sample_rate_hz);

  // fill audio frame info
  audio_frame->sample_rate_hz_ = send_sample_rate_hz;
  audio_frame->num_channels_ = send_num_channels;
  audio_frame->samples_per_channel_ = samples_per_channel;

  // send to audio stream sender
  SendFrame(std::move(audio_frame));
  return OK;
}

status_t AudioFlinger::GetPlaybackData(void* audio_data,
                                       uint32_t bytes_per_sample,
                                       uint32_t num_samples_per_channel,
                                       uint32_t num_channels,
                                       uint32_t sample_rate_hz) {
  return OK;
}

void AudioFlinger::SendFrame(std::unique_ptr<AudioFrame> audio_frame) {
  lock_guard l(&sender_lock_);
  if (senders_.empty())
    return;

  auto it = senders_.begin();
  while (++it != senders_.end()) {
    auto audio_frame_copy = std::make_unique<AudioFrame>();
    audio_frame_copy->CopyFrom(*audio_frame);
    (*it)->SendAudioData(std::move(audio_frame_copy));
  }
  // Send the original frame to the first stream w/o copying.
  (*senders_.begin())->SendAudioData(std::move(audio_frame));
}

void AudioFlinger::UpdateSender(
    std::vector<std::shared_ptr<AudioSendStream>> senders) {
  AVE_LOG(LS_ERROR) << "AudioFlinger::UpdateSender";
  lock_guard l(&sender_lock_);
  senders_ = senders;

  int sample_rate_hz = 0;
  size_t num_channels = 0;

  for (auto& sender : senders) {
    sample_rate_hz = std::max(sample_rate_hz, sender->sample_rate_hz());
    num_channels = std::max(num_channels, sender->num_channels());
  }

  sample_rate_hz_ = sample_rate_hz;
  num_channels_ = num_channels;
}

}  // namespace ave
