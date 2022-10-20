/*
 * main.cc
 * Copyright (C) 2021 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "app_config.h"
#include "base/logging.h"
#include "conductor.h"

using avp::AppConfig;
using avp::oc::Conductor;

int main() {
  avp::LogMessage::LogToDebug(avp::LogSeverity::LS_VERBOSE);

  const char* configFile = "/etc/open-camera/open_camera.ini";

  AppConfig config = AppConfig::CreateAppConfigFromFile(configFile);

  auto conductor = std::make_shared<Conductor>(config);
  conductor->init();

  conductor->waitingFinished();

  return 0;
}
