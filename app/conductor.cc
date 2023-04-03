/*
 * conductor.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "conductor.h"
#include <memory>

#include "base/checks.h"
#include "base/logging.h"
#include "common/message.h"
#include "media/media_service.h"

namespace avp {
namespace oc {

Conductor::Conductor(AppConfig appConfig)
    : config_(appConfig),
      looper_(std::make_shared<Looper>()),
      video_source_(std::make_shared<H264FileSource>("data/h264.bin")) {
  DCHECK(!config_.error);
  looper_->setName("conductor");
}

Conductor::~Conductor() {
  looper_->unregisterHandler(id());
  looper_->stop();
}

status_t Conductor::Init() {
  status_t ret = OK;
  looper_->start();
  looper_->registerHandler(shared_from_this());

  auto msg = std::make_shared<Message>(kWhatRtspNotify, shared_from_this());
  rtsp_server_ = std::make_shared<RtspServer>(msg);

  if ((ret = rtsp_server_->Init()) != OK) {
    return ret;
  }

  msg = std::make_shared<Message>(kWhatOnvifNotify, shared_from_this());
  onvif_server_ = std::make_shared<OnvifServer>(config_, msg);

  if ((ret = onvif_server_->Init()) != OK) {
    return ret;
  }

  msg = std::make_shared<Message>(kWhatMediaServiceNotify, shared_from_this());
  media_service_ = std::make_shared<MediaService>(config_, msg);
  if ((ret = media_service_->Init()) != OK) {
    return ret;
  }

  rtsp_server_->AddMediaSource(video_source_);
  return ret;
}

status_t Conductor::Start() {
  auto msg = std::make_shared<Message>(kWhatStart, shared_from_this());
  msg->post();
  return OK;
}

status_t Conductor::Stop() {
  auto msg = std::make_shared<Message>(kWhatStop, shared_from_this());
  msg->post();
  return OK;
}

void Conductor::WaitingFinished() {
  std::unique_lock<std::mutex> l(mutex_);
  condition_.wait(l);
}

void Conductor::SignalFinished() {
  condition_.notify_all();
}

void Conductor::OnRtspNotify(const std::shared_ptr<Message>& msg) {
  int32_t what;
  CHECK(msg->findInt32("what", &what));
  switch (what) {
    // TODO
  }
}

void Conductor::OnOnvifNotify(const std::shared_ptr<Message>& msg) {
  int32_t what;
  CHECK(msg->findInt32("what", &what));
  switch (what) {
    // TODO
  }
}
void Conductor::OnStart(const std::shared_ptr<Message>& msg) {
  rtsp_server_->Start();
  onvif_server_->Start();
  media_service_->Start();
}
void Conductor::OnStop(const std::shared_ptr<Message>& msg) {}

void Conductor::onMessageReceived(const std::shared_ptr<Message>& msg) {
  switch (msg->what()) {
    case kWhatStart: {
      OnStart(msg);
      break;
    }

    case kWhatStop: {
      OnStop(msg);
      break;
    }

    case kWhatRtspNotify: {
      OnRtspNotify(msg);
      break;
    }

    case kWhatOnvifNotify: {
      OnOnvifNotify(msg);
      break;
    }
  }
}

}  // namespace oc
}  // namespace avp
