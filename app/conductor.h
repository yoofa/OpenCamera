/*
 * conductor.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include <condition_variable>
#include <memory>
#include <mutex>

#include "app_config.h"
#include "common/handler.h"
#include "common/looper.h"
#include "common/message.h"
#include "onvif/onvif_server.h"
#include "rtsp/rtsp_server.h"

namespace avp {
namespace oc {

class Conductor : public Handler {
 public:
  Conductor(AppConfig appConfig);
  virtual ~Conductor();

  void init();
  void waitingFinished();

 private:
  void signalFinished();
  void onRtspNotify(const std::shared_ptr<Message>& message);
  void onOnvifNotify(const std::shared_ptr<Message>& message);
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  enum {
    kWhatRtspNotify = 'rtsp',
    kWhatOnvifNotify = 'onvf',
  };

  AppConfig mConfig;
  std::shared_ptr<Looper> mLooper;
  std::shared_ptr<RtspServer> mRtspServer;
  std::shared_ptr<OnvifServer> mOnvifServer;

  std::mutex mLock;
  std::condition_variable mCondition;
};
}  // namespace oc
}  // namespace avp

#endif /*conductor_t_H */
