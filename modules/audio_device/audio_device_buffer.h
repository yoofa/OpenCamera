/*
 * audio_device_buffer.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_DEVICE_BUFFER_H
#define AUDIO_DEVICE_BUFFER_H

#include "api/audio/audio_device.h"
#include "base/buffer.h"
#include "base/sequence_checker.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/thread_annotation.h"
#include "base/types.h"
#include "modules/audio_device/audio_device_defines.h"

namespace ave {

using ::ave::base::BufferT;
using ::ave::base::SequenceChecker;

// Delta times between two successive playout callbacks are limited to this
// value before added to an internal array.
const size_t kMaxDeltaTimeInMs = 500;
// TODO(henrika): remove when no longer used by external client.
const size_t kMaxBufferSizeBytes = 3840;  // 10ms in stereo @ 96kHz

class AudioDeviceBuffer {
 public:
  explicit AudioDeviceBuffer(base::TaskRunnerFactory* task_runner_factory);
  virtual ~AudioDeviceBuffer();

  int32_t RegisterAudioCallback(AudioTransport* audio_callback);

  void StartPlayout();
  void StartRecording();
  void StopPlayout();
  void StopRecording();

  int32_t SetRecordingSampleRate(uint32_t fsHz);
  int32_t SetPlayoutSampleRate(uint32_t fsHz);
  uint32_t RecordingSampleRate() const;
  uint32_t PlayoutSampleRate() const;

  int32_t SetRecordingChannels(size_t channels);
  int32_t SetPlayoutChannels(size_t channels);
  size_t RecordingChannels() const;
  size_t PlayoutChannels() const;

  virtual int32_t SetRecordedBuffer(const void* audio_buffer,
                                    size_t samples_per_channel);
  virtual void SetVQEData(int play_delay_ms, int rec_delay_ms);
  virtual int32_t DeliverRecordedData();
  uint32_t NewMicLevel() const;

  virtual int32_t RequestPlayoutData(size_t samples_per_channel);
  virtual int32_t GetPlayoutData(void* audio_buffer);

 private:
  // Starts/stops periodic logging of audio stats.
  void StartPeriodicLogging();
  void StopPeriodicLogging();

  // Main thread on which this object is created.
  SequenceChecker main_thread_checker_;

  // Raw pointer to AudioCallback instance. Supplied to RegisterAudioCallback()
  // and it must outlive this object. It is not possible to change this member
  // while any media is active. It is possible to start media without calling
  // RegisterAudioCallback() but that will lead to ignored audio callbacks in
  // both directions where native audio will be active but no audio samples will
  // be transported.
  AudioTransport* audio_cb_;

  // Sample rate in Hertz. Accessed atomically.
  std::atomic<uint32_t> rec_sample_rate_;
  std::atomic<uint32_t> play_sample_rate_;

  // Number of audio channels. Accessed atomically.
  std::atomic<size_t> rec_channels_;
  std::atomic<size_t> play_channels_;

  // Keeps track of if playout/recording are active or not. A combination
  // of these states are used to determine when to start and stop the timer.
  // Only used on the creating thread and not used to control any media flow.
  bool playing_ GUARDED_BY(main_thread_checker_);
  bool recording_ GUARDED_BY(main_thread_checker_);

  // Buffer used for audio samples to be played out. Size can be changed
  // dynamically. The 16-bit samples are interleaved, hence the size is
  // proportional to the number of channels.
  BufferT<int16_t> play_buffer_;

  // Byte buffer used for recorded audio samples. Size can be changed
  // dynamically.
  BufferT<int16_t> rec_buffer_;

  // Delay values used by the AEC.
  int play_delay_ms_;
  int rec_delay_ms_;

  // Time stamps of when playout and recording starts.
  int64_t play_start_time_ GUARDED_BY(main_thread_checker_);
  int64_t rec_start_time_ GUARDED_BY(main_thread_checker_);

// Should *never* be defined in production builds. Only used for testing.
// When defined, the output signal will be replaced by a sinus tone at 440Hz.
#ifdef AUDIO_DEVICE_PLAYS_SINUS_TONE
  double phase_;
#endif
};
}  // namespace ave

#endif /* !AUDIO_DEVICE_BUFFER_H */
