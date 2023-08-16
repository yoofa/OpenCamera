/*
 * media_service.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_SERVICE_H
#define MEDIA_SERVICE_H

#include <memory>

#include "api/video_codecs/video_encoder_factory.h"
#include "app/app_config.h"
#include "base/constructor_magic.h"
#include "base/errors.h"
#include "common/handler.h"
#include "common/looper.h"
#include "common/message.h"
#include "media/file_sink.h"
#include "media/media_worker.h"
#include "media/video_capturer.h"

namespace avp {

class MediaService : public Handler {
 public:
  MediaService(AppConfig appConfig, std::shared_ptr<Message> notify);
  ~MediaService();

  status_t Init();
  status_t Start();
  status_t Stop();

  void AddVideoSource(
      std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>&
          video_source,
      int32_t stream_id,
      CodecId codec_id,
      int32_t min_bitrate,
      int32_t max_bitrate);

  void AddVideoSink(
      const std::shared_ptr<VideoSinkInterface<EncodedImage>>& video_sink,
      int32_t stream_id);

  void RequesteKeyFrame();

  enum {
    kWhatStart = 'strt',
    kWhatStop = 'stop',

    kWhatEnableMotionDetector = 'emot',

    kWhatAddVideoSource = 'svid',
    kWhatRemoveVideoSource = 'rvid',
    kWhatAddEncodedVideoSink = 'senc',
    kWhatRemoveEncodedVideoSink = 'renc',

    kWhatRequestKeyFrame = 'rkey',

  };

 private:
  struct VideoCapturerPair {
    std::shared_ptr<VideoCapturer> capturer;
    int32_t id;
  };

  uint32_t GenerateStreamId();
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  AppConfig app_config_;
  std::shared_ptr<Message> notify_;
  std::shared_ptr<Looper> looper_;
  std::shared_ptr<Message> media_info_;

  uint32_t max_stream_id_;
  std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>
      video_source_;

  std::unique_ptr<VideoEncoderFactory> tmp_factory_;
  VideoEncoderFactory* video_encoder_factory_;
  std::shared_ptr<VideoCapturer> video_capturer_;
  std::vector<std::unique_ptr<MediaWorker>> media_workers_;

  std::vector<VideoCapturerPair> video_capturers_;

  std::shared_ptr<FileSink<EncodedImage>> file_sink_;
  std::shared_ptr<FileSink<EncodedImage>> file_sink2_;

  AVP_DISALLOW_COPY_AND_ASSIGN(MediaService);
};

}  // namespace avp

#endif /* !MEDIA_SERVICE_H */
