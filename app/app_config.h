/*
 * app_config.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/types.h"
#include "third_party/inih/src/INIReader.h"

namespace avp {
struct AppConfig {
  /************ oc info *************/
  float version;

  /************** rtsp **************/

  /************* rtsp **************/
  int onvif_port;
  std::string onvif_user;
  std::string onvif_password;

  // Device Information
  std::string manufacturer;
  std::string model;
  std::string firmware_version;
  std::string serial_number;
  std::string hardware_id;

  std::vector<std::string> scopes;
  std::string eth_ifs;

  // Stream Infomation
  std::string name;
  int width;
  int height;
  std::string stream_url;
  std::string snapurl;
  std::string stream_type;

  // PTZ Infomation
  bool ptz_enable;
  std::string move_left;
  std::string move_right;
  std::string move_up;
  std::string move_down;
  std::string move_stop;
  std::string move_preset;

  // no parse error
  bool error;

  static inline AppConfig CreateAppConfigFromFile(const char* path) {
    AppConfig appConfig;
    INIReader reader(path);
    if (reader.ParseError() != 0) {
      appConfig.error = true;
      return appConfig;
    }
    appConfig.error = false;

    appConfig.version = reader.GetFloat("oc", "version", 0.0);

    // onvif device
    appConfig.onvif_port = reader.GetInteger("onvif", "onvif_port", 0);
    appConfig.onvif_user = reader.Get("onvif", "onvif_user", "admin");
    appConfig.onvif_password = reader.Get("onvif", "onvif_password", "admin");
    appConfig.manufacturer =
        reader.Get("onvif", "manufacturer", "_manufacturer");
    appConfig.model = reader.Get("onvif", "model", "_model");
    appConfig.firmware_version =
        reader.Get("onvif", "firmware_version", "_firmware_version");
    appConfig.serial_number =
        reader.Get("onvif", "serial_number", "_serial_number");
    appConfig.manufacturer = reader.Get("onvif", "hardware_id", "_hardware_id");
    appConfig.scopes.push_back("onvif://www.onvif.org/name/OpenCamera");
    appConfig.scopes.push_back("onvif://www.onvif.org/Profile/S");
    appConfig.eth_ifs = reader.Get("onvif", "eth_ifs", "eth0");

    // onvif media
    appConfig.name = reader.Get("onvif", "stream_name", "RTSP");
    appConfig.width = reader.GetInteger("onvif", "width", 1920);
    appConfig.height = reader.GetInteger("onvif", "height", 1080);
    appConfig.stream_url =
        reader.Get("onvif", "stream_url", "rtsp://127.0.0.1:8554/live");
    appConfig.snapurl = reader.Get("onvif", "snapurl", "https://google.com");
    appConfig.stream_type = reader.Get("onvif", "stream_type", "H264");

    LOG(LS_INFO) << "stream:" << appConfig.stream_url
                 << ", name:" << appConfig.name;

    // onvif ptz
    appConfig.ptz_enable = reader.GetBoolean("ptz", "ptz_enable", false);
    appConfig.move_left =
        reader.Get("ptz", "ptz_move_left", "oc_ptz_move_left");
    appConfig.move_right =
        reader.Get("ptz", "ptz_move_right", "oc_ptz_move_right");
    appConfig.move_up = reader.Get("ptz", "ptz_move_up", "oc_ptz_move_up");
    appConfig.move_down =
        reader.Get("ptz", "ptz_move_down", "oc_ptz_move_down");
    appConfig.move_stop =
        reader.Get("ptz", "ptz_move_stop", "oc_ptz_move_stop");
    appConfig.move_preset =
        reader.Get("ptz", "ptz_move_preset", "oc_ptz_move_preset");

    return appConfig;
  }
};
}  // namespace avp

#endif /* !APP_CONFIG_H */
