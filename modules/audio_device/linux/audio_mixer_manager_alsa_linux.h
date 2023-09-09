/*
 * audio_mixer_manager_alsa_linux.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_MIXER_MANAGER_ALSA_LINUX_H
#define AUDIO_MIXER_MANAGER_ALSA_LINUX_H

#include <alsa/asoundlib.h>

#include "api/audio/audio_device.h"
#include "base/mutex.h"
#include "modules/audio_device/audio_device_defines.h"
#include "modules/audio_device/linux/alsasymboltable_linux.h"

namespace avp {

class AudioMixerManagerLinuxALSA {
 public:
  int32_t OpenSpeaker(char* deviceName) EXCLUDES(mutex_);
  int32_t OpenMicrophone(char* deviceName) EXCLUDES(mutex_);
  int32_t SetSpeakerVolume(uint32_t volume) EXCLUDES(mutex_);
  int32_t SpeakerVolume(uint32_t& volume) const;
  int32_t MaxSpeakerVolume(uint32_t& maxVolume) const;
  int32_t MinSpeakerVolume(uint32_t& minVolume) const;
  int32_t SpeakerVolumeIsAvailable(bool& available);
  int32_t SpeakerMuteIsAvailable(bool& available);
  int32_t SetSpeakerMute(bool enable) EXCLUDES(mutex_);
  int32_t SpeakerMute(bool& enabled) const;
  int32_t MicrophoneMuteIsAvailable(bool& available);
  int32_t SetMicrophoneMute(bool enable) EXCLUDES(mutex_);
  int32_t MicrophoneMute(bool& enabled) const;
  int32_t MicrophoneVolumeIsAvailable(bool& available);
  int32_t SetMicrophoneVolume(uint32_t volume) EXCLUDES(mutex_);
  int32_t MicrophoneVolume(uint32_t& volume) const;
  int32_t MaxMicrophoneVolume(uint32_t& maxVolume) const;
  int32_t MinMicrophoneVolume(uint32_t& minVolume) const;
  int32_t Close() EXCLUDES(mutex_);
  int32_t CloseSpeaker() EXCLUDES(mutex_);
  int32_t CloseMicrophone() EXCLUDES(mutex_);
  bool SpeakerIsInitialized() const;
  bool MicrophoneIsInitialized() const;

 public:
  AudioMixerManagerLinuxALSA();
  ~AudioMixerManagerLinuxALSA();

 private:
  int32_t CloseSpeakerLocked() REQUIRES(mutex_);
  int32_t CloseMicrophoneLocked() REQUIRES(mutex_);
  int32_t LoadMicMixerElement() const;
  int32_t LoadSpeakerMixerElement() const;
  void GetControlName(char* controlName, char* deviceName) const;

 private:
  Mutex mutex_;
  mutable snd_mixer_t* _outputMixerHandle;
  char _outputMixerStr[kAdmMaxDeviceNameSize];
  mutable snd_mixer_t* _inputMixerHandle;
  char _inputMixerStr[kAdmMaxDeviceNameSize];
  mutable snd_mixer_elem_t* _outputMixerElement;
  mutable snd_mixer_elem_t* _inputMixerElement;
};

}  // namespace avp

#endif /* !AUDIO_MIXER_MANAGER_ALSA_LINUX_H */
