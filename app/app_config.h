/*
 * app_config.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "base/types.h"

namespace avp {
struct AppConfig {
  bool health() { return true; }

  static AppConfig CreateAppConfigFromFile(const char* path) {
    AppConfig appConfig;
    return appConfig;
  }
};
}  // namespace avp

#endif /* !APP_CONFIG_H */
