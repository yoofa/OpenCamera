/*
 * onvif_server.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef ONVIF_SERVER_H
#define ONVIF_SERVER_H

#include <memory>
#include <thread>

#include "app/app_config.h"
#include "base/types.h"
#include "common/handler.h"
#include "common/message.h"

struct soap;
class ServiceContext;
namespace ave {

class OnvifDeviceBindingService;
class OnvifMediaBindingService;
class OnvifPTZBindingService;

class OnvifServer : public Handler {
 public:
  OnvifServer(AppConfig appConfig, std::shared_ptr<Message> notify);
  virtual ~OnvifServer();

  status_t Init();
  status_t Start();
  status_t Stop();

  enum {
    kWhatStart = 'star',
    kWhatStop = 'stop',

  };

 private:
  friend class OnvifDeviceBindingService;
  friend class OnvifMediaBindingService;
  friend class OnvifPTZBindingService;

  std::shared_ptr<Looper> looper_;
  std::shared_ptr<Message> notify_;

  AppConfig app_config_;
  std::unique_ptr<ServiceContext> service_info_;
  std::unique_ptr<soap, void (*)(soap*)> soap_;
  std::unique_ptr<OnvifDeviceBindingService> device_service_;
  std::unique_ptr<OnvifMediaBindingService> media_service_;
  std::unique_ptr<OnvifPTZBindingService> ptz_service_;

  std::thread soap_thread_;

  void onMessageReceived(const std::shared_ptr<Message>& msg) override;
  void onStart(const std::shared_ptr<Message>& msg);
  void onStop(const std::shared_ptr<Message>& msg);
};
}  // namespace ave

#endif /* !ONVIF_SERVER_H */
