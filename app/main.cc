/*
 * main.cc
 * Copyright (C) 2021 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include <getopt.h>
#include <string>

#include "app_config.h"
#include "base/logging.h"
#include "conductor.h"

using avp::AppConfig;
using avp::oc::Conductor;

#define DEFAULT_CONFIG_PATH "/etc/open-camera/open_camera.ini"

namespace LongOpts {
enum {
  help = 'h',
  config = 'c',
};
}  // namespace LongOpts

static const char* help_str =
    " ===============  Help  ===============\n"
    "  -c,  --config     [value]   choose the config file\n"
    "  -h,  --help                 Display this help\n\n";

static const char* short_opts = "hc:";
static struct option long_options[] = {
    {"help", no_argument, 0, LongOpts::help},
    {"config", required_argument, 0, LongOpts::config},
    {0, 0, 0, 0}};

int main(int argc, char** argv) {
  avp::LogMessage::LogToDebug(avp::LogSeverity::LS_VERBOSE);

  std::string configFile(DEFAULT_CONFIG_PATH);
  int opt;
  while ((opt = getopt_long(argc, argv, short_opts, long_options, NULL)) !=
         -1) {
    switch (opt) {
      case LongOpts::help: {
        puts(help_str);
        exit(0);
        break;
      }
      case LongOpts::config: {
        configFile = optarg;
        break;
      }
      default: {
        puts("Usage: somnus -h");
        exit(-1);
      }
    }
  }

  AppConfig config = AppConfig::CreateAppConfigFromFile(configFile.c_str());

  LOG(avp::LS_INFO) << "app version: " << config.version;

  auto conductor = std::make_shared<Conductor>(config);
  conductor->init();

  conductor->waitingFinished();

  return 0;
}
