/*
 * rtsp_server.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "rtsp_server.h"

#include <memory>

#include "base/logging.h"
#include "common/message.h"

namespace avp {

RtspServer::RtspServer(std::shared_ptr<Message> notify)
    : mNotify(std::move(notify)) {}

RtspServer::~RtspServer() {}

void RtspServer::init() {}

void RtspServer::start() {}

void RtspServer::stop() {}

}  // namespace avp
