/*
 * default_audio_device.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef DEFAULT_AUDIO_DEVICE_H
#define DEFAULT_AUDIO_DEVICE_H

#include "api/audio/audio_device.h"
#include "base/task_util/task.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "modules/audio_device/audio_device_generic.h"

namespace ave {
class DefaultAudioDevice : public AudioDevice {
 public:
  static std::unique_ptr<AudioDevice> Create(
      base::TaskRunnerFactory* task_runner_factory,
      AudioLayer interface = AudioLayer::kLinuxAlsa);

  explicit DefaultAudioDevice(base::TaskRunnerFactory* task_runner_factory,
                              AudioLayer audio_layer);
  virtual ~DefaultAudioDevice();

  // Retrieve the currently utilized audio layer
  virtual int32_t ActivedAudioLayer(AudioDevice::AudioLayer& audioLayer) const;
  virtual status_t RegisterAudioCallback(AudioTransport* callback);

  // Main initializaton and termination
  virtual status_t Init();
  virtual status_t Terminate();
  virtual bool Initialized() const;

  // Device enumeration
  virtual int16_t PlayoutDevices();
  virtual int16_t RecordingDevices();
  virtual int32_t PlayoutDeviceName(uint16_t index,
                                    char name[kAdmMaxDeviceNameSize],
                                    char guid[kAdmMaxGuidSize]);
  virtual int32_t RecordingDeviceName(uint16_t index,
                                      char name[kAdmMaxDeviceNameSize],
                                      char guid[kAdmMaxGuidSize]);

  // Device selection
  virtual int32_t SetPlayoutDevice(uint16_t index);
  virtual int32_t SetRecordingDevice(uint16_t index);

  // Audio transport initialization
  virtual int32_t PlayoutIsAvailable(bool& available);
  virtual int32_t InitPlayout();
  virtual bool PlayoutIsInitialized() const;
  virtual int32_t RecordingIsAvailable(bool& available);
  virtual int32_t InitRecording();
  virtual bool RecordingIsInitialized() const;

  // Audio transport control
  virtual int32_t StartPlayout();
  virtual int32_t StopPlayout();
  virtual bool Playing() const;
  virtual int32_t StartRecording();
  virtual int32_t StopRecording();
  virtual bool Recording() const;

  // Audio mixer initialization
  virtual int32_t InitSpeaker();
  virtual bool SpeakerIsInitialized() const;
  virtual int32_t InitMicrophone();
  virtual bool MicrophoneIsInitialized() const;

  // Speaker volume controls
  virtual int32_t SpeakerVolumeIsAvailable(bool& available);
  virtual int32_t SetSpeakerVolume(uint32_t volume);
  virtual int32_t SpeakerVolume(uint32_t& volume) const;
  virtual int32_t MaxSpeakerVolume(uint32_t& maxVolume) const;
  virtual int32_t MinSpeakerVolume(uint32_t& minVolume) const;

  // Microphone volume controls
  virtual int32_t MicrophoneVolumeIsAvailable(bool& available);
  virtual int32_t SetMicrophoneVolume(uint32_t volume);
  virtual int32_t MicrophoneVolume(uint32_t& volume) const;
  virtual int32_t MaxMicrophoneVolume(uint32_t& maxVolume) const;
  virtual int32_t MinMicrophoneVolume(uint32_t& minVolume) const;

  // Speaker mute control
  virtual int32_t SpeakerMuteIsAvailable(bool& available);
  virtual int32_t SetSpeakerMute(bool enable);
  virtual int32_t SpeakerMute(bool& enabled) const;

  // Microphone mute control
  virtual int32_t MicrophoneMuteIsAvailable(bool& available);
  virtual int32_t SetMicrophoneMute(bool enable);
  virtual int32_t MicrophoneMute(bool& enabled) const;

  // Stereo support
  virtual int32_t StereoPlayoutIsAvailable(bool& available);
  virtual int32_t SetStereoPlayout(bool enable);
  virtual int32_t StereoPlayout(bool& enabled) const;
  virtual int32_t StereoRecordingIsAvailable(bool& available);
  virtual int32_t SetStereoRecording(bool enable);
  virtual int32_t StereoRecording(bool& enabled) const;

  // Delay information and control
  virtual int32_t PlayoutDelay(uint16_t& delayMS) const;

  // Android only
  virtual bool BuiltInAECIsAvailable() const;
  virtual bool BuiltInAGCIsAvailable() const;
  virtual bool BuiltInNSIsAvailable() const;

  // Windows Core Audio and Android only.
  virtual int32_t EnableBuiltInAEC(bool enable);
  virtual int32_t EnableBuiltInAGC(bool enable);
  virtual int32_t EnableBuiltInNS(bool enable);

  // Play underrun count.
  // virtual int32_t GetPlayoutUnderrunCount() const;

 private:
  int32_t CreatePlatformSpecificObjects();
  int32_t AttachAudioBuffer();

  base::TaskRunner task_runner_;
  AudioLayer audio_layer_;

  std::unique_ptr<AudioDeviceGeneric> audio_device_;
  std::unique_ptr<AudioDeviceBuffer> audio_device_buffer_;

  bool initialized_;
};

}  // namespace ave

#endif /* !DEFAULT_AUDIO_DEVICE_H */
