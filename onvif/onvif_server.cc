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

namespace ave {

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
    : looper_(std::make_shared<Looper>()),
      notify_(std::move(notify)),
      app_config_(appConfig),
      service_info_(new ServiceContext()),
      soap_(soap_new(),
            [](soap* soap) {
              soap_destroy(soap);
              soap_end(soap);
              soap_done(soap);
            }),
      device_service_(new OnvifDeviceBindingService(soap_.get())),
      media_service_(new OnvifMediaBindingService(soap_.get())),
      ptz_service_(new OnvifPTZBindingService(soap_.get())) {
  AVE_DCHECK(!app_config_.error);
  looper_->setName("OnvifServer");
}

OnvifServer::~OnvifServer() {
  looper_->unregisterHandler(id());
  looper_->stop();
}

status_t OnvifServer::Init() {
  looper_->start();
  looper_->registerHandler(shared_from_this());

  status_t err = convertConfig(service_info_.get(), &app_config_);
  if (err) {
    AVE_LOG(LS_ERROR) << "init config error";
    return err;
  }

  if (!soap_valid_socket(
          soap_bind(soap_.get(), NULL, service_info_->port, 10))) {
    std::stringstream ss;
    soap_stream_fault(soap_.get(), ss);
    AVE_LOG(LS_ERROR) << "bind socket error, port:" << service_info_->port
                      << ", " << ss.str();
    return UNKNOWN_ERROR;
  }

  // timeout in sec
  soap_->send_timeout = 3;
  soap_->recv_timeout = 3;

  soap_->user = this;

  return OK;
}

status_t OnvifServer::Start() {
  auto msg = std::make_shared<Message>(kWhatStart, shared_from_this());
  msg->post();
  return OK;
}

status_t OnvifServer::Stop() {
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
  soap_thread_ = std::thread([this]() {
    // FIXME stop thread
    while (true) {
      std::stringstream ss;
      // wait new client
      if (!soap_valid_socket(soap_accept(soap_.get()))) {
        ss.clear();
        soap_stream_fault(soap_.get(), ss);
        AVE_LOG(LS_ERROR) << "start with no valid socket." << ss.str();
        // TODO notify OnvifServer
      }

      // process ervices
      if (soap_begin_serve(soap_.get())) {
        ss.clear();
        soap_stream_fault(soap_.get(), ss);
        AVE_LOG(LS_ERROR) << ss.str();
      } else if (device_service_->dispatch() != SOAP_NO_METHOD) {
        ss.clear();
        soap_send_fault(soap_.get());
        soap_stream_fault(soap_.get(), ss);
        AVE_LOG(LS_ERROR) << ss.str();
      } else if (media_service_->dispatch() != SOAP_NO_METHOD) {
        ss.clear();
        soap_send_fault(soap_.get());
        soap_stream_fault(soap_.get(), ss);
        AVE_LOG(LS_ERROR) << ss.str();
      } else if (ptz_service_->dispatch() != SOAP_NO_METHOD) {
        ss.clear();
        soap_send_fault(soap_.get());
        soap_stream_fault(soap_.get(), ss);
        AVE_LOG(LS_ERROR) << ss.str();
      } else {
        AVE_LOG(LS_WARNING) << "UNKNOWN SERVICE";
      }

      soap_destroy(soap_.get());
      soap_end(soap_.get());
    }
  });
}

void OnvifServer::onStop(const std::shared_ptr<Message>& msg) {}

}  // namespace ave
