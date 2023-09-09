/*
 * audio_device.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_DEVICE_H
#define AUDIO_DEVICE_H

#include "base/types.h"
#include "modules/audio_device/audio_device_defines.h"

namespace avp {

class AudioTransport;
class AudioDeviceBuffer;
class AudioDevice {
 public:
  enum AudioLayer {
    kDefaultAudio = 0,
    kLinuxAlsa,
    kLinuxPulseAudio,
  };

  virtual ~AudioDevice() = default;

  // Retrieve the currently utilized audio layer
  virtual int32_t ActivedAudioLayer(
      AudioDevice::AudioLayer& audioLayer) const = 0;
  virtual status_t RegisterAudioCallback(AudioTransport* callback) = 0;

  // Main initializaton and termination
  virtual status_t Init() = 0;
  virtual status_t Terminate() = 0;
  virtual bool Initialized() const = 0;

  // Device enumeration
  virtual int16_t PlayoutDevices() = 0;
  virtual int16_t RecordingDevices() = 0;
  virtual int32_t PlayoutDeviceName(uint16_t index,
                                    char name[kAdmMaxDeviceNameSize],
                                    char guid[kAdmMaxGuidSize]) = 0;
  virtual int32_t RecordingDeviceName(uint16_t index,
                                      char name[kAdmMaxDeviceNameSize],
                                      char guid[kAdmMaxGuidSize]) = 0;

  // Device selection
  virtual int32_t SetPlayoutDevice(uint16_t index) = 0;
  virtual int32_t SetRecordingDevice(uint16_t index) = 0;

  // Audio transport initialization
  virtual int32_t PlayoutIsAvailable(bool& available) = 0;
  virtual int32_t InitPlayout() = 0;
  virtual bool PlayoutIsInitialized() const = 0;
  virtual int32_t RecordingIsAvailable(bool& available) = 0;
  virtual int32_t InitRecording() = 0;
  virtual bool RecordingIsInitialized() const = 0;

  // Audio transport control
  virtual int32_t StartPlayout() = 0;
  virtual int32_t StopPlayout() = 0;
  virtual bool Playing() const = 0;
  virtual int32_t StartRecording() = 0;
  virtual int32_t StopRecording() = 0;
  virtual bool Recording() const = 0;

  // Audio mixer initialization
  virtual int32_t InitSpeaker() = 0;
  virtual bool SpeakerIsInitialized() const = 0;
  virtual int32_t InitMicrophone() = 0;
  virtual bool MicrophoneIsInitialized() const = 0;

  // Speaker volume controls
  virtual int32_t SpeakerVolumeIsAvailable(bool& available) = 0;
  virtual int32_t SetSpeakerVolume(uint32_t volume) = 0;
  virtual int32_t SpeakerVolume(uint32_t& volume) const = 0;
  virtual int32_t MaxSpeakerVolume(uint32_t& maxVolume) const = 0;
  virtual int32_t MinSpeakerVolume(uint32_t& minVolume) const = 0;

  // Microphone volume controls
  virtual int32_t MicrophoneVolumeIsAvailable(bool& available) = 0;
  virtual int32_t SetMicrophoneVolume(uint32_t volume) = 0;
  virtual int32_t MicrophoneVolume(uint32_t& volume) const = 0;
  virtual int32_t MaxMicrophoneVolume(uint32_t& maxVolume) const = 0;
  virtual int32_t MinMicrophoneVolume(uint32_t& minVolume) const = 0;

  // Speaker mute control
  virtual int32_t SpeakerMuteIsAvailable(bool& available) = 0;
  virtual int32_t SetSpeakerMute(bool enable) = 0;
  virtual int32_t SpeakerMute(bool& enabled) const = 0;

  // Microphone mute control
  virtual int32_t MicrophoneMuteIsAvailable(bool& available) = 0;
  virtual int32_t SetMicrophoneMute(bool enable) = 0;
  virtual int32_t MicrophoneMute(bool& enabled) const = 0;

  // Stereo support
  virtual int32_t StereoPlayoutIsAvailable(bool& available) = 0;
  virtual int32_t SetStereoPlayout(bool enable) = 0;
  virtual int32_t StereoPlayout(bool& enabled) const = 0;
  virtual int32_t StereoRecordingIsAvailable(bool& available) = 0;
  virtual int32_t SetStereoRecording(bool enable) = 0;
  virtual int32_t StereoRecording(bool& enabled) const = 0;

  // Delay information and control
  virtual int32_t PlayoutDelay(uint16_t& delayMS) const = 0;

  // Android only
  virtual bool BuiltInAECIsAvailable() const = 0;
  virtual bool BuiltInAGCIsAvailable() const = 0;
  virtual bool BuiltInNSIsAvailable() const = 0;

  // Windows Core Audio and Android only.
  virtual int32_t EnableBuiltInAEC(bool enable) = 0;
  virtual int32_t EnableBuiltInAGC(bool enable) = 0;
  virtual int32_t EnableBuiltInNS(bool enable) = 0;

  // Play underrun count.
  // virtual int32_t GetPlayoutUnderrunCount() const;
};

}  // namespace avp

#endif /* !AUDIO_DEVICE_H */
