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
#include "media/media_service.h"
#include "onvif/onvif_server.h"
#include "rtsp/h264_file_source.h"
#include "rtsp/rtsp_server.h"

namespace ave {
namespace oc {

class Conductor : public Handler {
 public:
  Conductor(AppConfig appConfig);
  virtual ~Conductor();

  status_t Init();
  status_t Start();
  status_t Stop();
  void WaitingFinished();

  void AddCameraSource();

 private:
  struct VideoCapturerPair {
    std::shared_ptr<VideoCapturer> capturer;
    int32_t id;
  };
  uint32_t GenerateStreamId();
  void SignalFinished();
  void OnRtspNotify(const std::shared_ptr<Message>& message);
  void OnOnvifNotify(const std::shared_ptr<Message>& message);

  void OnStart(const std::shared_ptr<Message>& message);
  void OnStop(const std::shared_ptr<Message>& message);

  // handler
  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  enum {
    kWhatStart = 'star',
    kWhatStop = 'stop',
    kWhatRtspNotify = 'rtsp',
    kWhatOnvifNotify = 'onvf',
    kWhatMediaServiceNotify = 'meds',

    kWhatAddVideoSource = 'avss',

    kWhatAddVideoSink = 'avsk',
  };

  AppConfig config_;
  std::shared_ptr<Looper> looper_;
  std::shared_ptr<RtspServer> rtsp_server_;
  std::shared_ptr<OnvifServer> onvif_server_;
  std::shared_ptr<MediaService> media_service_;

  uint32_t max_stream_id_;
  std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>
      camera_source_;
  std::shared_ptr<MediaSource> video_source_;
  std::vector<VideoCapturerPair> video_capturers_;

  std::mutex mutex_;
  std::condition_variable condition_;
};
}  // namespace oc
}  // namespace ave

#endif /*conductor_t_H */
