/*
 * conductor.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "conductor.h"

#include "base/checks.h"
#include "base/logging.h"
#include "common/message.h"

namespace avp {
namespace oc {

Conductor::Conductor(AppConfig appConfig)
    : mConfig(appConfig),
      mLooper(std::make_shared<Looper>()),
      mVideoSource(std::make_shared<H264FileSource>("data/h264.bin")) {
  DCHECK(mConfig.noError);
  mLooper->setName("conductor");
}

Conductor::~Conductor() {
  mLooper->unregisterHandler(id());
  mLooper->stop();
}

void Conductor::init() {
  mLooper->start();
  mLooper->registerHandler(shared_from_this());

  auto msg = std::make_shared<Message>(kWhatRtspNotify, shared_from_this());
  mRtspServer = std::make_shared<RtspServer>(msg);

  msg = std::make_shared<Message>(kWhatOnvifNotify, shared_from_this());
  mOnvifServer = std::make_shared<OnvifServer>(mConfig, msg);

  mRtspServer->init();
  mOnvifServer->init();

  mRtspServer->addMediaSource(mVideoSource);
}

void Conductor::start() {
  auto msg = std::make_shared<Message>(kWhatStart, shared_from_this());
  msg->post();
}

void Conductor::stop() {
  auto msg = std::make_shared<Message>(kWhatStop, shared_from_this());
  msg->post();
}

void Conductor::waitingFinished() {
  std::unique_lock<std::mutex> l(mLock);
  mCondition.wait(l);
}

void Conductor::signalFinished() {
  mCondition.notify_all();
}

void Conductor::onRtspNotify(const std::shared_ptr<Message>& msg) {
  int32_t what;
  CHECK(msg->findInt32("what", &what));
  switch (what) {
    // TODO
  }
}

void Conductor::onOnvifNotify(const std::shared_ptr<Message>& msg) {
  int32_t what;
  CHECK(msg->findInt32("what", &what));
  switch (what) {
    // TODO
  }
}
void Conductor::onStart(const std::shared_ptr<Message>& msg) {
  mRtspServer->start();
  mOnvifServer->start();
}
void Conductor::onStop(const std::shared_ptr<Message>& msg) {}

void Conductor::onMessageReceived(const std::shared_ptr<Message>& msg) {
  switch (msg->what()) {
    case kWhatStart: {
      onStart(msg);
      break;
    }

    case kWhatStop: {
      onStop(msg);
      break;
    }

    case kWhatRtspNotify: {
      onRtspNotify(msg);
      break;
    }

    case kWhatOnvifNotify: {
      onOnvifNotify(msg);
      break;
    }
  }
}

}  // namespace oc
}  // namespace avp
