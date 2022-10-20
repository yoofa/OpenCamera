/*
 * app_config.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "base/types.h"
#include "third_party/inih/src/INIReader.h"

namespace avp {
struct AppConfig {
  // oc info
  float version;

  // rtsp

  // onvif

  // no parth error
  bool noError;

  static AppConfig CreateAppConfigFromFile(const char* path) {
    AppConfig appConfig;
    INIReader reader(path);
    if (reader.ParseError() != 0) {
      appConfig.noError = false;
      return appConfig;
    }
    appConfig.noError = true;

    appConfig.version = reader.GetFloat("oc", "version", 0.0);

    return appConfig;
  }
};
}  // namespace avp

#endif /* !APP_CONFIG_H */
