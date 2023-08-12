/*
 * media_service.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_SERVICE_H
#define MEDIA_SERVICE_H

#include <memory>

#include "app/app_config.h"
#include "base/constructor_magic.h"
#include "base/errors.h"
#include "common/handler.h"
#include "common/looper.h"
#include "common/message.h"
#include "media/file_sink.h"
#include "media/video_capturer.h"

namespace avp {

class MediaService : public Handler {
 public:
  MediaService(AppConfig appConfig, std::shared_ptr<Message> notify);
  ~MediaService();

  status_t Init();
  status_t Start();
  status_t Stop();

  enum {
    kWhatStart = 'strt',
    kWhatStop = 'stop',
    kWhatAddCapturerSink = 'scap',
    kWhatEnableMotionDetector = 'emot',

  };

 private:
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  AppConfig app_config_;
  std::shared_ptr<Message> notify_;
  std::shared_ptr<Looper> looper_;
  std::shared_ptr<Message> media_info_;

  std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>
      video_source_;

  std::shared_ptr<VideoCapturer> video_capturer_;

  std::shared_ptr<FileSink> row_file_sink_;

  AVP_DISALLOW_COPY_AND_ASSIGN(MediaService);
};

}  // namespace avp

#endif /* !MEDIA_SERVICE_H */
