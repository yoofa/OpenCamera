/*
 * audio_frame.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_FRAME_H
#define AUDIO_FRAME_H

#include <optional>

#include "base/types.h"
#include "common/channel_layout.h"

namespace avp {
class AudioFrame {
 public:
  enum : size_t {
    // Stereo, 32 kHz, 120 ms (2 * 32 * 120)
    // Stereo, 192 kHz, 20 ms (2 * 192 * 20)
    kMaxDataSizeSamples = 7680,
    kMaxDataSizeBytes = kMaxDataSizeSamples * sizeof(int16_t),
  };
  AudioFrame();

  AudioFrame(const AudioFrame&) = delete;
  AudioFrame& operator=(const AudioFrame&) = delete;

  // Resets all members to their default state.
  void Reset();
  // Same as Reset(), but leaves mute state unchanged. Muting a frame requires
  // the buffer to be zeroed on the next call to mutable_data(). Callers
  // intending to write to the buffer immediately after Reset() can instead use
  // ResetWithoutMuting() to skip this wasteful zeroing.
  void ResetWithoutMuting();

  void UpdateFrame(uint32_t timestamp,
                   const int16_t* data,
                   size_t samples_per_channel,
                   int sample_rate_hz,
                   size_t num_channels = 1);

  void CopyFrom(const AudioFrame& src);

  // data() returns a zeroed static buffer if the frame is muted.
  // mutable_frame() always returns a non-static buffer; the first call to
  // mutable_frame() zeros the non-static buffer and marks the frame unmuted.
  const int16_t* data() const;
  int16_t* mutable_data();

  // Prefer to mute frames using AudioFrameOperations::Mute.
  void Mute();
  // Frame is muted by default.
  bool muted() const;

  size_t max_16bit_samples() const { return kMaxDataSizeSamples; }
  size_t samples_per_channel() const { return samples_per_channel_; }
  size_t num_channels() const { return num_channels_; }
  ChannelLayout channel_layout() const { return channel_layout_; }
  int sample_rate_hz() const { return sample_rate_hz_; }

  void set_absolute_capture_timestamp_ms(
      int64_t absolute_capture_time_stamp_ms) {
    absolute_capture_timestamp_ms_ = absolute_capture_time_stamp_ms;
  }

  std::optional<int64_t> absolute_capture_timestamp_ms() const {
    return absolute_capture_timestamp_ms_;
  }

  // RTP timestamp of the first sample in the AudioFrame.
  uint32_t timestamp_ = 0;
  // Time since the first frame in milliseconds.
  // -1 represents an uninitialized value.
  int64_t elapsed_time_ms_ = -1;
  // NTP time of the estimated capture time in local timebase in milliseconds.
  // -1 represents an uninitialized value.
  int64_t ntp_time_ms_ = -1;
  size_t samples_per_channel_ = 0;
  int sample_rate_hz_ = 0;
  size_t num_channels_ = 0;
  ChannelLayout channel_layout_ = CHANNEL_LAYOUT_NONE;

 private:
  // A permanently zeroed out buffer to represent muted frames. This is a
  // header-only class, so the only way to avoid creating a separate empty
  // buffer per translation unit is to wrap a static in an inline function.
  static const int16_t* empty_data();

  int16_t data_[kMaxDataSizeSamples];
  bool muted_ = true;
  // Absolute capture timestamp when this audio frame was originally captured.
  // This is only valid for audio frames captured on this machine.
  std::optional<int64_t> absolute_capture_timestamp_ms_;
};
}  // namespace avp

#endif /* !AUDIO_FRAME_H */
