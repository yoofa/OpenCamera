/*
 * alsa.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "api/audio/default_audio_device.h"
#include "base/logging.h"
#include "base/task_util/default_task_runner_factory.h"
#include "base/task_util/task_runner_factory.h"

using namespace avp;
using namespace base;

int main(int argc, char* argv[]) {
  auto task_runner_factory = base::CreateDefaultTaskRunnerFactory();
  auto audio_device = DefaultAudioDevice::Create(
      task_runner_factory.get(), AudioDevice::AudioLayer::kLinuxAlsa);
  int32_t ret = audio_device->Init();
  LOG(LS_INFO) << "Init: " << ret;

  AudioDevice::AudioLayer layer = AudioDevice::AudioLayer::kDefaultAudio;
  ret = audio_device->ActivedAudioLayer(layer);
  LOG(LS_INFO) << "ActivedAudioLayer return " << ret << ", layer: " << layer;

  char device_name[kAdmMaxDeviceNameSize];
  char unique_id[kAdmMaxGuidSize];

  LOG(LS_INFO) << "PlayoutDevices: " << audio_device->PlayoutDevices();
  for (int i = 0; i < audio_device->PlayoutDevices(); i++) {
    audio_device->PlayoutDeviceName(i, device_name, unique_id);
    LOG(LS_INFO) << "PlayoutDeviceName: " << device_name;
    LOG(LS_INFO) << "PlayoutDeviceUniqueId: " << unique_id;
  }

  LOG(LS_INFO) << "RecordingDevices: " << audio_device->RecordingDevices();
  for (int i = 0; i < audio_device->RecordingDevices(); i++) {
    audio_device->RecordingDeviceName(i, device_name, unique_id);
    LOG(LS_INFO) << "RecordingDeviceName: " << device_name;
    LOG(LS_INFO) << "RecordingDeviceUniqueId: " << unique_id;
  }

  if (audio_device->SetRecordingDevice(4) != 0) {
    LOG(LS_ERROR) << "AudioDevice SetRecordingDevice failed";
  }

  if (audio_device->InitMicrophone() != 0) {
    LOG(LS_ERROR) << "AudioDevice InitMicrophone failed";
  }

  return 0;
}
