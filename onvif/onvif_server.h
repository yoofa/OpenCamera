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
namespace avp {

class OnvifDeviceBindingService;
class OnvifMediaBindingService;
class OnvifPTZBindingService;

class OnvifServer : public Handler {
 public:
  OnvifServer(AppConfig appConfig, std::shared_ptr<Message> notify);
  virtual ~OnvifServer();

  status_t init();
  status_t start();
  status_t stop();

  enum {
    kWhatStart = 'star',
    kWhatStop = 'stop',

  };

 private:
  friend class OnvifDeviceBindingService;
  friend class OnvifMediaBindingService;
  friend class OnvifPTZBindingService;

  std::shared_ptr<Looper> mLooper;
  std::shared_ptr<Message> mNotify;

  std::unique_ptr<ServiceContext> mServicesInfo;
  std::unique_ptr<soap, void (*)(soap*)> mSoap;
  std::unique_ptr<OnvifDeviceBindingService> mDeviceSerivce;
  std::unique_ptr<OnvifMediaBindingService> mMediaService;
  std::unique_ptr<OnvifPTZBindingService> mPTZService;

  std::thread mSoapThread;

  void onMessageReceived(const std::shared_ptr<Message>& msg) override;
  void onStart(const std::shared_ptr<Message>& msg);
  void onStop(const std::shared_ptr<Message>& msg);
};
}  // namespace avp

#endif /* !ONVIF_SERVER_H */
