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
#include "media/v4l2_video_source.h"
#include "xop/rtsp.h"

namespace avp {
namespace oc {
namespace {
using VideoSource = VideoSourceInterface<std::shared_ptr<VideoFrame>>;
using EncodedVideoSink = VideoSinkInterface<EncodedImage>;
}  // namespace

Conductor::Conductor(AppConfig appConfig)
    : config_(appConfig),
      looper_(std::make_shared<Looper>()),
      max_stream_id_(0),
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

uint32_t Conductor::GenerateStreamId() {
  return ++max_stream_id_;
}

void Conductor::AddCameraSource() {
  auto media_info = std::make_shared<Message>();
  media_info->setString("v4l2-dev", config_.v4l2_device);
  camera_source_ = V4L2VideoSource::Create(media_info);

  int32_t id = GenerateStreamId();
  video_capturers_.push_back({std::make_unique<VideoCapturer>(nullptr), id});
  video_capturers_.back().capturer->SetVideoSource(camera_source_.get(),
                                                   VideoSinkWants());

  auto msg = std::make_shared<Message>(kWhatAddVideoSource, shared_from_this());
  msg->setObject("video_source", video_capturers_.back().capturer);
  msg->setInt32("stream_id", id);
  msg->setInt32("codec_format",
                static_cast<int32_t>(CodecId::AV_CODEC_ID_H264));
  msg->setInt32("min_kbps", 300);
  msg->setInt32("max_kbps", 10000);
  msg->post();
}

void Conductor::OnRtspNotify(const std::shared_ptr<Message>& msg) {
  int32_t what;
  CHECK(msg->findInt32("what", &what));
  switch (what) {
    case RtspServer::kWhatVideoSinkAdded: {
      LOG(LS_INFO) << "kWhatVideoSinkAdded";
      int32_t stream_id;
      DCHECK(msg->findInt32("stream_id", &stream_id));

      std::shared_ptr<MessageObject> obj;
      CHECK(msg->findObject("encoded_video_sink", obj));
      std::shared_ptr<EncodedVideoSink> encoded_video_sink =
          std::dynamic_pointer_cast<EncodedVideoSink>(obj);
      DCHECK(encoded_video_sink != nullptr);

      media_service_->AddVideoSink(encoded_video_sink, stream_id);

      LOG(LS_INFO) << "kWhatVideoSinkAdded";

      break;
    }
    case RtspServer::kWhatClientConnectedNotify: {
      media_service_->RequesteKeyFrame();
      break;
    }
    default: {
      break;
    }
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

  AddCameraSource();
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
    case kWhatAddVideoSource: {
      int32_t id;
      CHECK(msg->findInt32("stream_id", &id));
      int32_t codec_id;
      CHECK(msg->findInt32("codec_format", &codec_id));
      int32_t min_bitrate;
      CHECK(msg->findInt32("min_kbps", &min_bitrate));
      int32_t max_bitrate;
      CHECK(msg->findInt32("max_kbps", &max_bitrate));

      std::shared_ptr<MessageObject> obj;
      CHECK(msg->findObject("video_source", obj));
      std::shared_ptr<VideoSource> video_source =
          std::dynamic_pointer_cast<VideoSource>(obj);
      DCHECK(video_source != nullptr);

      media_service_->AddVideoSource(video_source, id,
                                     static_cast<CodecId>(codec_id),
                                     min_bitrate, max_bitrate);

      rtsp_server_->RequestVideoSink(id, static_cast<CodecId>(codec_id));
      break;
    }

    case kWhatAddVideoSink: {
      break;
    }
  }
}

}  // namespace oc
}  // namespace avp
