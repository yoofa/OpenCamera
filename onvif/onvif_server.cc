/*
 * onvif_server.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "onvif_server.h"

#include <memory>

#include "common/message.h"

namespace avp {

OnvifServer::OnvifServer(std::shared_ptr<Message> notify)
    : mNotify(std::move(notify)) {}

OnvifServer::~OnvifServer() {}

void OnvifServer::init() {}

void OnvifServer::start() {}

void OnvifServer::stop() {}

}  // namespace avp
