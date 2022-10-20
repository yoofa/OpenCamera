/*
 * rtsp_server.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <memory>

#include "common/message.h"

namespace avp {
class RtspServer {
 public:
  RtspServer(std::shared_ptr<Message> notify);
  virtual ~RtspServer();
  void init();
  void start();
  void stop();

 private:
  std::shared_ptr<Message> mNotify;
};
}  // namespace avp

#endif /* !RTSPSERVER_H */
