/*
 * media_service.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "media/media_service.h"

#include <memory>
#include <utility>
#include "app/app_config.h"
#include "base/checks.h"
#include "base/errors.h"
#include "base/logging.h"
#include "common/looper.h"
#include "common/message.h"
#include "media/file_sink.h"
#include "media/v4l2_video_source.h"

namespace avp {

MediaService::MediaService(AppConfig appConfig, std::shared_ptr<Message> notify)
    : app_config_(appConfig),
      notify_(std::move(notify)),
      looper_(new Looper()),
      media_info_(std::make_shared<Message>()),
      row_file_sink_(std::make_shared<FileSink>("media_service.frame")) {
  looper_->setName("MediaService");
}

MediaService::~MediaService() {}

status_t MediaService::Init() {
  looper_->start();
  looper_->registerHandler(shared_from_this());

  media_info_->setString("v4l2-dev", app_config_.v4l2_device);

  video_capturer_ = std::make_shared<VideoCapturer>(media_info_);
  video_source_ = V4L2VideoSource::Create(media_info_);

  video_capturer_->SetVideoSource(video_source_.get(), VideoSinkWants());
  video_capturer_->AddOrUpdateSink(row_file_sink_.get(), VideoSinkWants());

  return OK;
}

status_t MediaService::Start() {
  return OK;
}

status_t MediaService::Stop() {
  return OK;
}

void MediaService::onMessageReceived(const std::shared_ptr<Message>& message) {
  switch (message->what()) {
    case kWhatStart: {
      break;
    }
    case kWhatStop: {
      break;
    }
    case kWhatAddCapturerSink: {
      std::shared_ptr<MessageObject> obj;
      // DCHECK(message->findObject("video_sink", obj));
      //       video_capturer_->AddVideoSink(video_sink);
      break;
    }
    case kWhatEnableMotionDetector: {
      break;
    }
    default: {
      break;
    }
  }
}

}  // namespace avp
