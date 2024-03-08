/*
 * default_audio_device.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "default_audio_device.h"

#include <memory>

#include "base/checks.h"
#include "base/logging.h"
#include "base/sequence_checker.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "modules/audio_device/audio_device_buffer.h"
#include "modules/audio_device/linux/audio_device_alsa_linux.h"

namespace {
#define AVE_CHECKinitialized_() \
  {                             \
    if (!initialized_) {        \
      return -1;                \
    }                           \
  }

#define AVE_CHECKinitialized__BOOL() \
  {                                  \
    if (!initialized_) {             \
      return false;                  \
    }                                \
  }
}  // namespace

namespace ave {

std::unique_ptr<AudioDevice> DefaultAudioDevice::Create(
    base::TaskRunnerFactory* task_runner_factory,
    AudioLayer audio_layer) {
  // TODO(youfa) now only support linux alsa
  if (audio_layer != AudioLayer::kLinuxAlsa) {
    return nullptr;
  }

  auto device =
      std::make_unique<DefaultAudioDevice>(task_runner_factory, audio_layer);

  if (device->CreatePlatformSpecificObjects() < 0) {
    return nullptr;
  }

  if (device->AttachAudioBuffer() < 0) {
    return nullptr;
  }
  return device;
}

DefaultAudioDevice::DefaultAudioDevice(
    base::TaskRunnerFactory* task_runner_factory,
    AudioLayer audio_layer)
    : task_runner_(task_runner_factory->CreateTaskRunner(
          "DefaultAudioDevice",
          base::TaskRunnerFactory::Priority::HIGH)),
      audio_layer_(audio_layer),
      audio_device_(nullptr),
      audio_device_buffer_(
          std::make_unique<AudioDeviceBuffer>(task_runner_factory)) {
  //
}

DefaultAudioDevice::~DefaultAudioDevice() {}

int32_t DefaultAudioDevice::CreatePlatformSpecificObjects() {
  if (audio_device_ != nullptr) {
    AVE_LOG(LS_INFO) << "Reusing provided audio device";
    return 0;
  }

  if (audio_layer_ == AudioLayer::kLinuxAlsa) {
    audio_device_.reset(new AudioDeviceLinuxALSA());
  }

  if (audio_device_ == nullptr) {
    AVE_LOG(LS_ERROR) << "Failed to create platform specific audio device";
    return -1;
  }
  return 0;
}

int32_t DefaultAudioDevice::AttachAudioBuffer() {
  audio_device_->AttachAudioBuffer(audio_device_buffer_.get());
  return 0;
}

// Retrieve the currently utilized audio layer
int32_t DefaultAudioDevice::ActivedAudioLayer(
    AudioDevice::AudioLayer& audioLayer) const {
  if (audio_device_->ActiveAudioLayer(audioLayer) < 0) {
    return -1;
  }
  return 0;
}
status_t DefaultAudioDevice::RegisterAudioCallback(AudioTransport* callback) {
  return audio_device_buffer_->RegisterAudioCallback(callback);
}

// Main initializaton and termination
status_t DefaultAudioDevice::Init() {
  if (initialized_) {
    return OK;
  }
  AVE_DCHECK(audio_device_);
  auto status = audio_device_->Init();
  if (status != OK) {
    return status;
  }

  initialized_ = true;
  return OK;
}
status_t DefaultAudioDevice::Terminate() {
  if (!initialized_) {
    return OK;
  }
  if (audio_device_->Terminate() < 0) {
    return UNKNOWN_ERROR;
  }
  initialized_ = false;
  return OK;
}

bool DefaultAudioDevice::Initialized() const {
  return initialized_;
}

// Device enumeration
int16_t DefaultAudioDevice::PlayoutDevices() {
  AVE_CHECKinitialized_();
  return (int16_t)audio_device_->PlayoutDevices();
}

int16_t DefaultAudioDevice::RecordingDevices() {
  AVE_CHECKinitialized_();
  return (int16_t)audio_device_->RecordingDevices();
}

int32_t DefaultAudioDevice::PlayoutDeviceName(uint16_t index,
                                              char name[kAdmMaxDeviceNameSize],
                                              char guid[kAdmMaxGuidSize]) {
  AVE_CHECKinitialized_();
  if (name == nullptr) {
    return -1;
  }
  if (audio_device_->PlayoutDeviceName(index, name, guid) == -1) {
    return -1;
  }
  return 0;
}

int32_t DefaultAudioDevice::RecordingDeviceName(
    uint16_t index,
    char name[kAdmMaxDeviceNameSize],
    char guid[kAdmMaxGuidSize]) {
  AVE_CHECKinitialized_();
  if (name == nullptr) {
    return -1;
  }
  if (audio_device_->RecordingDeviceName(index, name, guid) == -1) {
    return -1;
  }
  return 0;
}

// Device selection
int32_t DefaultAudioDevice::SetPlayoutDevice(uint16_t index) {
  AVE_CHECKinitialized_();
  return audio_device_->SetPlayoutDevice(index);
}
int32_t DefaultAudioDevice::SetRecordingDevice(uint16_t index) {
  AVE_CHECKinitialized_();
  return audio_device_->SetRecordingDevice(index);
}

// Audio transport initialization
int32_t DefaultAudioDevice::PlayoutIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->PlayoutIsAvailable(available);
}

int32_t DefaultAudioDevice::InitPlayout() {
  AVE_CHECKinitialized_();
  if (PlayoutIsInitialized()) {
    return 0;
  }
  return audio_device_->InitPlayout();
}

bool DefaultAudioDevice::PlayoutIsInitialized() const {
  AVE_CHECKinitialized__BOOL();
  return audio_device_->PlayoutIsInitialized();
}

int32_t DefaultAudioDevice::RecordingIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->RecordingIsAvailable(available);
}

int32_t DefaultAudioDevice::InitRecording() {
  AVE_CHECKinitialized_();
  if (RecordingIsInitialized()) {
    return 0;
  }
  return audio_device_->InitRecording();
}

bool DefaultAudioDevice::RecordingIsInitialized() const {
  AVE_CHECKinitialized__BOOL();
  return audio_device_->RecordingIsInitialized();
}

// Audio transport control
int32_t DefaultAudioDevice::StartPlayout() {
  AVE_CHECKinitialized_();
  if (Playing()) {
    return 0;
  }
  audio_device_buffer_->StartPlayout();
  return audio_device_->StartPlayout();
}
int32_t DefaultAudioDevice::StopPlayout() {
  AVE_CHECKinitialized_();
  int32_t ret = audio_device_->StopPlayout();
  audio_device_buffer_->StopPlayout();
  return ret;
}
bool DefaultAudioDevice::Playing() const {
  AVE_CHECKinitialized__BOOL();
  return audio_device_->Playing();
}
int32_t DefaultAudioDevice::StartRecording() {
  AVE_CHECKinitialized_();
  if (Recording()) {
    return 0;
  }
  audio_device_buffer_->StartRecording();
  return audio_device_->StartRecording();
}
int32_t DefaultAudioDevice::StopRecording() {
  AVE_CHECKinitialized_();
  auto result = audio_device_->StopRecording();
  audio_device_buffer_->StopRecording();
  return result;
}
bool DefaultAudioDevice::Recording() const {
  AVE_CHECKinitialized__BOOL();
  return audio_device_->Recording();
}

// Audio mixer initialization
int32_t DefaultAudioDevice::InitSpeaker() {
  AVE_CHECKinitialized_();
  return audio_device_->InitSpeaker();
}

bool DefaultAudioDevice::SpeakerIsInitialized() const {
  AVE_CHECKinitialized__BOOL();
  return audio_device_->SpeakerIsInitialized();
}

int32_t DefaultAudioDevice::InitMicrophone() {
  AVE_CHECKinitialized_();
  return audio_device_->InitMicrophone();
}

bool DefaultAudioDevice::MicrophoneIsInitialized() const {
  AVE_CHECKinitialized__BOOL();
  return audio_device_->MicrophoneIsInitialized();
}

// Speaker volume controls
int32_t DefaultAudioDevice::SpeakerVolumeIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->SpeakerVolumeIsAvailable(available);
}
int32_t DefaultAudioDevice::SetSpeakerVolume(uint32_t volume) {
  AVE_CHECKinitialized_();
  return audio_device_->SetSpeakerVolume(volume);
}
int32_t DefaultAudioDevice::SpeakerVolume(uint32_t& volume) const {
  AVE_CHECKinitialized_();
  return audio_device_->SpeakerVolume(volume);
}
int32_t DefaultAudioDevice::MaxSpeakerVolume(uint32_t& maxVolume) const {
  AVE_CHECKinitialized_();
  return audio_device_->MaxSpeakerVolume(maxVolume);
}
int32_t DefaultAudioDevice::MinSpeakerVolume(uint32_t& minVolume) const {
  AVE_CHECKinitialized_();
  return audio_device_->MinSpeakerVolume(minVolume);
}

// Microphone volume controls
int32_t DefaultAudioDevice::MicrophoneVolumeIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->MicrophoneVolumeIsAvailable(available);
}
int32_t DefaultAudioDevice::SetMicrophoneVolume(uint32_t volume) {
  AVE_CHECKinitialized_();
  return audio_device_->SetMicrophoneVolume(volume);
}
int32_t DefaultAudioDevice::MicrophoneVolume(uint32_t& volume) const {
  AVE_CHECKinitialized_();
  return audio_device_->MicrophoneVolume(volume);
}
int32_t DefaultAudioDevice::MaxMicrophoneVolume(uint32_t& maxVolume) const {
  AVE_CHECKinitialized_();
  return audio_device_->MaxMicrophoneVolume(maxVolume);
}
int32_t DefaultAudioDevice::MinMicrophoneVolume(uint32_t& minVolume) const {
  AVE_CHECKinitialized_();
  return audio_device_->MinMicrophoneVolume(minVolume);
}

// Speaker mute control
int32_t DefaultAudioDevice::SpeakerMuteIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->SpeakerMuteIsAvailable(available);
}
int32_t DefaultAudioDevice::SetSpeakerMute(bool enable) {
  AVE_CHECKinitialized_();
  return audio_device_->SetSpeakerMute(enable);
}
int32_t DefaultAudioDevice::SpeakerMute(bool& enabled) const {
  AVE_CHECKinitialized_();
  return audio_device_->SpeakerMute(enabled);
}

// Microphone mute control
int32_t DefaultAudioDevice::MicrophoneMuteIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->MicrophoneMuteIsAvailable(available);
}
int32_t DefaultAudioDevice::SetMicrophoneMute(bool enable) {
  AVE_CHECKinitialized_();
  return audio_device_->SetMicrophoneMute(enable);
}
int32_t DefaultAudioDevice::MicrophoneMute(bool& enabled) const {
  AVE_CHECKinitialized_();
  return audio_device_->MicrophoneMute(enabled);
}

// Stereo support
int32_t DefaultAudioDevice::StereoPlayoutIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->StereoPlayoutIsAvailable(available);
}
int32_t DefaultAudioDevice::SetStereoPlayout(bool enable) {
  AVE_CHECKinitialized_();
  return audio_device_->SetStereoPlayout(enable);
}
int32_t DefaultAudioDevice::StereoPlayout(bool& enabled) const {
  AVE_CHECKinitialized_();
  return audio_device_->StereoPlayout(enabled);
}
int32_t DefaultAudioDevice::StereoRecordingIsAvailable(bool& available) {
  AVE_CHECKinitialized_();
  return audio_device_->StereoRecordingIsAvailable(available);
}
int32_t DefaultAudioDevice::SetStereoRecording(bool enable) {
  AVE_CHECKinitialized_();
  return audio_device_->SetStereoRecording(enable);
}
int32_t DefaultAudioDevice::StereoRecording(bool& enabled) const {
  AVE_CHECKinitialized_();
  return audio_device_->StereoRecording(enabled);
  ;
}

// Delay information and control
int32_t DefaultAudioDevice::PlayoutDelay(uint16_t& delayMS) const {
  AVE_CHECKinitialized_();
  return audio_device_->PlayoutDelay(delayMS);
}

// Android only
bool DefaultAudioDevice::BuiltInAECIsAvailable() const {
  AVE_CHECKinitialized__BOOL();
  return false;
}

bool DefaultAudioDevice::BuiltInAGCIsAvailable() const {
  AVE_CHECKinitialized__BOOL();
  return false;
}

bool DefaultAudioDevice::BuiltInNSIsAvailable() const {
  AVE_CHECKinitialized__BOOL();
  return false;
}

// 3A
int32_t DefaultAudioDevice::EnableBuiltInAEC(bool enable) {
  AVE_CHECKinitialized_();
  return 0;
}
int32_t DefaultAudioDevice::EnableBuiltInAGC(bool enable) {
  AVE_CHECKinitialized_();
  return 0;
}
int32_t DefaultAudioDevice::EnableBuiltInNS(bool enable) {
  AVE_CHECKinitialized_();
  return 0;
}

}  // namespace ave
