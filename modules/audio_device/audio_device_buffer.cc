/*
 * audio_device_buffer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_device_buffer.h"

#include <string.h>

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "api/audio/audio_device.h"
#include "base/checks.h"
#include "base/logging.h"
#include "base/time_utils.h"
// #include "common_audio/signal_processing/include/signal_processing_library.h"
// #include "system_wrappers/include/metrics.h"

namespace ave {

using base::TimeMillis;
using base::TimeSince;

#ifdef AUDIO_DEVICE_PLAYS_SINUS_TONE
static const double k2Pi = 6.28318530717959;
#endif

AudioDeviceBuffer::AudioDeviceBuffer(
    base::TaskRunnerFactory* task_runner_factory)
    : audio_cb_(nullptr),
      rec_sample_rate_(0),
      play_sample_rate_(0),
      rec_channels_(0),
      play_channels_(0),
      playing_(false),
      recording_(false),
      play_delay_ms_(0),
      rec_delay_ms_(0),
      play_start_time_(0) {
  AVE_LOG(LS_DEBUG) << "AudioDeviceBuffer::ctor";
#ifdef AUDIO_DEVICE_PLAYS_SINUS_TONE
  phase_ = 0.0;
  AVE_LOG(WARNING) << "AUDIO_DEVICE_PLAYS_SINUS_TONE is defined!";
#endif
}

AudioDeviceBuffer::~AudioDeviceBuffer() {
  AVE_DCHECK_RUN_ON(&main_thread_checker_);
  AVE_DCHECK(!playing_);
  AVE_DCHECK(!recording_);
  AVE_LOG(LS_DEBUG) << "AudioDeviceBuffer::~dtor";
}

int32_t AudioDeviceBuffer::RegisterAudioCallback(
    AudioTransport* audio_callback) {
  AVE_DCHECK_RUN_ON(&main_thread_checker_);
  AVE_LOG(LS_DEBUG) << __FUNCTION__;
  if (playing_ || recording_) {
    AVE_LOG(LS_ERROR) << "Failed to set audio callback since media was active";
    return -1;
  }
  audio_cb_ = audio_callback;
  return 0;
}

void AudioDeviceBuffer::StartPlayout() {
  AVE_DCHECK_RUN_ON(&main_thread_checker_);
  if (playing_) {
    return;
  }
  AVE_LOG(LS_DEBUG) << __FUNCTION__;

  const int64_t now_time = TimeMillis();
  // Clear members that are only touched on the main (creating) thread.
  play_start_time_ = now_time;
  playing_ = true;
}

void AudioDeviceBuffer::StartRecording() {
  AVE_DCHECK_RUN_ON(&main_thread_checker_);
  if (recording_) {
    return;
  }
  AVE_LOG(LS_DEBUG) << __FUNCTION__;

  // Clear members that will be touched on the main (creating) thread.
  rec_start_time_ = TimeMillis();
  recording_ = true;
}

void AudioDeviceBuffer::StopPlayout() {
  AVE_DCHECK_RUN_ON(&main_thread_checker_);
  if (!playing_) {
    return;
  }
  AVE_LOG(LS_DEBUG) << __FUNCTION__;
  playing_ = false;

  AVE_LOG(LS_INFO) << "total playout time: " << TimeSince(play_start_time_);
}

void AudioDeviceBuffer::StopRecording() {
  AVE_DCHECK_RUN_ON(&main_thread_checker_);
  if (!recording_) {
    return;
  }
  AVE_LOG(LS_DEBUG) << __FUNCTION__;
  recording_ = false;

  const size_t time_since_start = TimeSince(rec_start_time_);
  AVE_LOG(LS_INFO) << "total recording time: " << time_since_start;
}

int32_t AudioDeviceBuffer::SetRecordingSampleRate(uint32_t fsHz) {
  AVE_LOG(LS_INFO) << "SetRecordingSampleRate(" << fsHz << ")";
  rec_sample_rate_ = fsHz;
  return 0;
}

int32_t AudioDeviceBuffer::SetPlayoutSampleRate(uint32_t fsHz) {
  AVE_LOG(LS_INFO) << "SetPlayoutSampleRate(" << fsHz << ")";
  play_sample_rate_ = fsHz;
  return 0;
}

uint32_t AudioDeviceBuffer::RecordingSampleRate() const {
  return rec_sample_rate_;
}

uint32_t AudioDeviceBuffer::PlayoutSampleRate() const {
  return play_sample_rate_;
}

int32_t AudioDeviceBuffer::SetRecordingChannels(size_t channels) {
  AVE_LOG(LS_INFO) << "SetRecordingChannels(" << channels << ")";
  rec_channels_ = channels;
  return 0;
}

int32_t AudioDeviceBuffer::SetPlayoutChannels(size_t channels) {
  AVE_LOG(LS_INFO) << "SetPlayoutChannels(" << channels << ")";
  play_channels_ = channels;
  return 0;
}

size_t AudioDeviceBuffer::RecordingChannels() const {
  return rec_channels_;
}

size_t AudioDeviceBuffer::PlayoutChannels() const {
  return play_channels_;
}

void AudioDeviceBuffer::SetVQEData(int play_delay_ms, int rec_delay_ms) {
  play_delay_ms_ = play_delay_ms;
  rec_delay_ms_ = rec_delay_ms;
}

int32_t AudioDeviceBuffer::SetRecordedBuffer(const void* audio_buffer,
                                             size_t samples_per_channel) {
  // Copy the complete input buffer to the local buffer.
  const size_t old_size = rec_buffer_.size();
  rec_buffer_.SetData(static_cast<const int16_t*>(audio_buffer),
                      rec_channels_ * samples_per_channel);
  // Keep track of the size of the recording buffer. Only updated when the
  // size changes, which is a rare event.
  if (old_size != rec_buffer_.size()) {
    AVE_LOG(LS_INFO) << "Size of recording buffer: " << rec_buffer_.size();
  }

  return 0;
}

int32_t AudioDeviceBuffer::DeliverRecordedData() {
  if (!audio_cb_) {
    AVE_LOG(LS_WARNING) << "Invalid audio callback";
    return 0;
  }
  const size_t frames_per_channel = rec_buffer_.size() / rec_channels_;
  const size_t bytes_per_frame = rec_channels_ * sizeof(int16_t);
  int32_t res = audio_cb_->DataIsRecorded(rec_buffer_.data(), bytes_per_frame,
                                          frames_per_channel, rec_channels_,
                                          rec_sample_rate_);
  if (res == -1) {
    AVE_LOG(LS_ERROR) << "RecordedDataIsAvailable() failed";
  }
  return 0;
}

int32_t AudioDeviceBuffer::RequestPlayoutData(size_t samples_per_channel) {
  // The consumer can change the requested size on the fly and we therefore
  // resize the buffer accordingly. Also takes place at the first call to this
  // method.
  const size_t total_samples = play_channels_ * samples_per_channel;
  if (play_buffer_.size() != total_samples) {
    play_buffer_.SetSize(total_samples);
    AVE_LOG(LS_INFO) << "Size of playout buffer: " << play_buffer_.size();
  }

  size_t num_samples_out(0);
  // It is currently supported to start playout without a valid audio
  // callback object. Leads to warning and silence.
  if (!audio_cb_) {
    AVE_LOG(LS_WARNING) << "Invalid audio callback";
    return 0;
  }

  // Retrieve new 16-bit PCM audio data using the audio callback instance.
  const size_t bytes_per_frame = play_channels_ * sizeof(int16_t);
  uint32_t res = audio_cb_->GetPlaybackData(
      play_buffer_.data(), bytes_per_frame, samples_per_channel, play_channels_,
      play_sample_rate_);
  if (res != 0) {
    AVE_LOG(LS_ERROR) << "NeedMorePlayData() failed";
  }

  return static_cast<int32_t>(num_samples_out / play_channels_);
}

int32_t AudioDeviceBuffer::GetPlayoutData(void* audio_buffer) {
  AVE_DCHECK_GT(play_buffer_.size(), 0);
#ifdef AUDIO_DEVICE_PLAYS_SINUS_TONE
  const double phase_increment =
      k2Pi * 440.0 / static_cast<double>(play_sample_rate_);
  int16_t* destination_r = reinterpret_cast<int16_t*>(audio_buffer);
  if (play_channels_ == 1) {
    for (size_t i = 0; i < play_buffer_.size(); ++i) {
      destination_r[i] = static_cast<int16_t>((sin(phase_) * (1 << 14)));
      phase_ += phase_increment;
    }
  } else if (play_channels_ == 2) {
    for (size_t i = 0; i < play_buffer_.size() / 2; ++i) {
      destination_r[2 * i] = destination_r[2 * i + 1] =
          static_cast<int16_t>((sin(phase_) * (1 << 14)));
      phase_ += phase_increment;
    }
  }
#else
  memcpy(audio_buffer, play_buffer_.data(),
         play_buffer_.size() * sizeof(int16_t));
#endif
  // Return samples per channel or number of frames.
  return static_cast<int32_t>(play_buffer_.size() / play_channels_);
}

}  // namespace ave
