/*
 * onvif_server.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "onvif_server.h"

#include <memory>

#include "app/app_config.h"
#include "base/checks.h"
#include "base/logging.h"
#include "common/message.h"
#include "onvif_device_binding_service.h"
#include "onvif_media_binding_service.h"
#include "onvif_ptz_binding_service.h"
#include "third_party/onvif_srvd/src/generated/DeviceBinding.nsmap"
#include "third_party/onvif_srvd/src/generated/soapH.h"
#include "third_party/onvif_srvd/src/src/ServiceContext.h"

namespace avp {

status_t convertConfig(ServiceContext* serviceContext, AppConfig* appConfig) {
  bool noerr = true;
  serviceContext->port = appConfig->onvif_port;
  serviceContext->user = appConfig->onvif_user;
  serviceContext->password = appConfig->onvif_password;
  serviceContext->manufacturer = appConfig->manufacturer;
  serviceContext->model = appConfig->model;
  serviceContext->firmware_version = appConfig->firmware_version;
  serviceContext->serial_number = appConfig->serial_number;
  serviceContext->hardware_id = appConfig->hardware_id;
  serviceContext->scopes = appConfig->scopes;

  serviceContext->eth_ifs.push_back(Eth_Dev_Param());
  if (serviceContext->eth_ifs.back().open(appConfig->eth_ifs.c_str()) != OK) {
    return UNKNOWN_ERROR;
  }

  StreamProfile streamProfile;
  noerr |= streamProfile.set_name(appConfig->name.c_str());
  noerr |= streamProfile.set_width(std::to_string(appConfig->width).c_str());
  noerr |= streamProfile.set_height(std::to_string(appConfig->height).c_str());
  noerr |= streamProfile.set_url(appConfig->stream_url.c_str());
  noerr |= streamProfile.set_snapurl(appConfig->snapurl.c_str());
  noerr |= streamProfile.set_type(appConfig->stream_type.c_str());
  noerr |= serviceContext->add_profile(streamProfile);

  serviceContext->get_ptz_node()->enable = appConfig->ptz_enable;
  noerr |= serviceContext->get_ptz_node()->set_move_left(
      appConfig->move_left.c_str());
  noerr |= serviceContext->get_ptz_node()->set_move_right(
      appConfig->move_right.c_str());
  noerr |=
      serviceContext->get_ptz_node()->set_move_up(appConfig->move_up.c_str());

  noerr |= serviceContext->get_ptz_node()->set_move_down(
      appConfig->move_down.c_str());
  noerr |= serviceContext->get_ptz_node()->set_move_stop(
      appConfig->move_stop.c_str());
  noerr |= serviceContext->get_ptz_node()->set_move_preset(
      appConfig->move_preset.c_str());

  return noerr ? OK : UNKNOWN_ERROR;
}

OnvifServer::OnvifServer(AppConfig appConfig, std::shared_ptr<Message> notify)
    : mLooper(std::make_shared<Looper>()),
      mNotify(std::move(notify)),
      mAppConfig(appConfig),
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
  DCHECK(!mAppConfig.error);
  mLooper->setName("OnvifServer");
}

OnvifServer::~OnvifServer() {
  mLooper->unregisterHandler(id());
  mLooper->stop();
}

status_t OnvifServer::init() {
  mLooper->start();
  mLooper->registerHandler(shared_from_this());

  status_t err = convertConfig(mServicesInfo.get(), &mAppConfig);
  if (err) {
    LOG(LS_ERROR) << "init config error";
    return err;
  }

  if (!soap_valid_socket(
          soap_bind(mSoap.get(), NULL, mServicesInfo->port, 10))) {
    std::stringstream ss;
    soap_stream_fault(mSoap.get(), ss);
    LOG(LS_ERROR) << "bind socket error, port:" << mServicesInfo->port << ", "
                  << ss.str();
    return UNKNOWN_ERROR;
  }

  // timeout in sec
  mSoap->send_timeout = 3;
  mSoap->recv_timeout = 3;

  mSoap->user = this;

  return OK;
}

status_t OnvifServer::start() {
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
      std::stringstream ss;
      // wait new client
      if (!soap_valid_socket(soap_accept(mSoap.get()))) {
        ss.clear();
        soap_stream_fault(mSoap.get(), ss);
        LOG(LS_ERROR) << "start with no valid socket." << ss.str();
        // TODO notify OnvifServer
      }

      // process ervices
      if (soap_begin_serve(mSoap.get())) {
        ss.clear();
        soap_stream_fault(mSoap.get(), ss);
        LOG(LS_ERROR) << ss.str();
      } else if (mDeviceSerivce->dispatch() != SOAP_NO_METHOD) {
        ss.clear();
        soap_send_fault(mSoap.get());
        soap_stream_fault(mSoap.get(), ss);
        LOG(LS_ERROR) << ss.str();
      } else if (mMediaService->dispatch() != SOAP_NO_METHOD) {
        ss.clear();
        soap_send_fault(mSoap.get());
        soap_stream_fault(mSoap.get(), ss);
        LOG(LS_ERROR) << ss.str();
      } else if (mPTZService->dispatch() != SOAP_NO_METHOD) {
        ss.clear();
        soap_send_fault(mSoap.get());
        soap_stream_fault(mSoap.get(), ss);
        LOG(LS_ERROR) << ss.str();
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
