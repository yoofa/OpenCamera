/*
 * onvif_server.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef ONVIF_SERVER_H
#define ONVIF_SERVER_H

#include <memory>

#include "common/message.h"

namespace avp {
class OnvifServer {
 public:
  OnvifServer(std::shared_ptr<Message> notify);
  virtual ~OnvifServer();

  void init();
  void start();
  void stop();

 private:
  std::shared_ptr<Message> mNotify;
};
}  // namespace avp

#endif /* !ONVIF_SERVER_H */
