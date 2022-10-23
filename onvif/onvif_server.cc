/*
 * onvif_server.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "onvif_server.h"

#include <memory>

#include "app/app_config.h"
#include "base/logging.h"
#include "common/message.h"
#include "onvif_device_binding_service.h"
#include "onvif_media_binding_service.h"
#include "onvif_ptz_binding_service.h"
#include "third_party/onvif_srvd/src/generated/DeviceBinding.nsmap"
#include "third_party/onvif_srvd/src/generated/soapH.h"
#include "third_party/onvif_srvd/src/src/ServiceContext.h"

namespace avp {

OnvifServer::OnvifServer(AppConfig appConfig, std::shared_ptr<Message> notify)
    : mLooper(std::make_shared<Looper>()),
      mNotify(std::move(notify)),
      mServicesInfo(new ServiceContext()),
      mSoap(soap_new(),
            [](soap* soap) {
              soap_destroy(soap);
              soap_end(soap);
              soap_done(soap);
            }),
      mDeviceSerivce(new OnvifDeviceBindingService(mSoap.get())),
      mMediaService(new OnvifMediaBindingService(mSoap.get())),
      mPTZService(new OnvifPTZBindingService(mSoap.get())) {
  mLooper->setName("OnvifServer");
  mServicesInfo->port = appConfig.onvifPort;
}

OnvifServer::~OnvifServer() {
  mLooper->unregisterHandler(id());
  mLooper->stop();
}

status_t OnvifServer::init() {
  LOG(LS_VERBOSE) << "init";

  mLooper->start();
  mLooper->registerHandler(shared_from_this());

  if (!soap_valid_socket(
          soap_bind(mSoap.get(), NULL, mServicesInfo->port, 10))) {
    soap_stream_fault(mSoap.get(), std::cerr);
    return UNKNOWN_ERROR;
  }

  // timeout in sec
  mSoap->send_timeout = 3;
  mSoap->recv_timeout = 3;

  mSoap->user = this;

  return OK;
}

status_t OnvifServer::start() {
  LOG(LS_VERBOSE) << "start";
  auto msg = std::make_shared<Message>(kWhatStart, shared_from_this());
  msg->post();
  return OK;
}

status_t OnvifServer::stop() {
  auto msg = std::make_shared<Message>(kWhatStop, shared_from_this());
  msg->post();
  return OK;
}

void OnvifServer::onMessageReceived(const std::shared_ptr<Message>& message) {
  switch (message->what()) {
    case kWhatStart: {
      onStart(message);
      break;
    }
    case kWhatStop: {
      onStop(message);
      break;
    }
  }
}

void OnvifServer::onStart(const std::shared_ptr<Message>& msg) {
  mSoapThread = std::thread([this]() {
    // FIXME stop thread
    while (true) {
      // wait new client
      if (!soap_valid_socket(soap_accept(mSoap.get()))) {
        soap_stream_fault(mSoap.get(), std::cerr);
        // TODO notify OnvifServer
      }

      // process ervices
      if (soap_begin_serve(mSoap.get())) {
        soap_stream_fault(mSoap.get(), std::cerr);
      } else if (mDeviceSerivce->dispatch() != SOAP_NO_METHOD) {
        soap_send_fault(mSoap.get());
        soap_stream_fault(mSoap.get(), std::cerr);
      } else if (mMediaService->dispatch() != SOAP_NO_METHOD) {
        soap_send_fault(mSoap.get());
        soap_stream_fault(mSoap.get(), std::cerr);
      } else if (mPTZService->dispatch() != SOAP_NO_METHOD) {
        soap_send_fault(mSoap.get());
        soap_stream_fault(mSoap.get(), std::cerr);
      } else {
        LOG(LS_WARNING) << "UNKNOWN SERVICE";
      }

      soap_destroy(mSoap.get());
      soap_end(mSoap.get());
    }
  });
}

void OnvifServer::onStop(const std::shared_ptr<Message>& msg) {}

}  // namespace avp
