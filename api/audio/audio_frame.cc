/*
 * audio_frame.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_frame.h"

#include "base/checks.h"
namespace avp {

AudioFrame::AudioFrame() {
  // Visual Studio doesn't like this in the class definition.
  static_assert(sizeof(data_) == kMaxDataSizeBytes, "kMaxDataSizeBytes");
}

void AudioFrame::Reset() {
  ResetWithoutMuting();
  muted_ = true;
}

void AudioFrame::ResetWithoutMuting() {
  timestamp_ = 0;
  elapsed_time_ms_ = -1;
  ntp_time_ms_ = -1;
  samples_per_channel_ = 0;
  sample_rate_hz_ = 0;
  num_channels_ = 0;
  channel_layout_ = CHANNEL_LAYOUT_NONE;
  absolute_capture_timestamp_ms_ = std::nullopt;
}

void AudioFrame::UpdateFrame(uint32_t timestamp,
                             const int16_t* data,
                             size_t samples_per_channel,
                             int sample_rate_hz,
                             size_t num_channels) {
  timestamp_ = timestamp;
  samples_per_channel_ = samples_per_channel;
  sample_rate_hz_ = sample_rate_hz;
  num_channels_ = num_channels;
  channel_layout_ = GuessChannelLayout(num_channels);
  if (channel_layout_ != CHANNEL_LAYOUT_UNSUPPORTED) {
    DCHECK_EQ(num_channels,
              (size_t)ChannelLayoutToChannelCount(channel_layout_));
  }

  const size_t length = samples_per_channel * num_channels;
  CHECK_LE(length, kMaxDataSizeSamples);
  if (data != nullptr) {
    memcpy(data_, data, sizeof(int16_t) * length);
    muted_ = false;
  } else {
    muted_ = true;
  }
}

void AudioFrame::CopyFrom(const AudioFrame& src) {
  if (this == &src)
    return;

  timestamp_ = src.timestamp_;
  elapsed_time_ms_ = src.elapsed_time_ms_;
  ntp_time_ms_ = src.ntp_time_ms_;
  muted_ = src.muted();
  samples_per_channel_ = src.samples_per_channel_;
  sample_rate_hz_ = src.sample_rate_hz_;
  num_channels_ = src.num_channels_;
  channel_layout_ = src.channel_layout_;
  absolute_capture_timestamp_ms_ = src.absolute_capture_timestamp_ms();

  const size_t length = samples_per_channel_ * num_channels_;
  CHECK_LE(length, kMaxDataSizeSamples);
  if (!src.muted()) {
    memcpy(data_, src.data(), sizeof(int16_t) * length);
    muted_ = false;
  }
}

const int16_t* AudioFrame::data() const {
  return muted_ ? empty_data() : data_;
}

int16_t* AudioFrame::mutable_data() {
  if (muted_) {
    memset(data_, 0, kMaxDataSizeBytes);
    muted_ = false;
  }
  return data_;
}

void AudioFrame::Mute() {
  muted_ = true;
}

bool AudioFrame::muted() const {
  return muted_;
}

// static
const int16_t* AudioFrame::empty_data() {
  static int16_t* null_data = new int16_t[kMaxDataSizeSamples]();
  return &null_data[0];
}

}  // namespace avp
