/*
 * video_capturer.cc
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_capturer.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <utility>

#include "base/checks.h"
#include "base/errors.h"
#include "base/logging.h"
#include "base/task_util/default_task_runner_factory.h"
#include "base/task_util/repeating_task.h"
#include "base/task_util/task_runner.h"
#include "common/codec_constants.h"
#include "common/looper.h"
#include "common/message.h"
#include "media/default_video_source_factory.h"

namespace avp {

VideoCapturer::VideoCapturer(std::shared_ptr<Looper> looper,
                             std::shared_ptr<Message> capture_info)
    : capture_info_(std::move(capture_info)),
      video_source_factory_(CreateDefaultVideoSourceFactory()),
      video_source_(video_source_factory_->CreateVideoSource(capture_info_)),
      looper_(std::move(looper)),
      task_runner_factory_(base::CreateDefaultTaskRunnerFactory()),
      task_runner_(std::make_unique<base::TaskRunner>(
          task_runner_factory_->CreateTaskRunner(
              "VideoCapturer",
              base::TaskRunnerFactory::Priority::NORMAL))),
      video_processor_(nullptr) {
  // ctor
}

VideoCapturer::~VideoCapturer() {}

status_t VideoCapturer::Init() {
  task_runner_->postTask([this]() {
    looper_->registerHandler(
        std::enable_shared_from_this<Handler>::shared_from_this());
    repeating_task_handler_ = RepeatingTaskHandle::DelayedStart(
        task_runner_->Get(), 1 * 1000 * 1000, [this]() {
          CaptureOneFrame();
          //  16ms, about 60fps
          return (uint64_t)(16 * 1000);
        });
  });

  return OK;
}

status_t VideoCapturer::Start() {
  task_runner_->postTask([this]() {
    MetaData metaData;
    metaData.setInt32(kKeyWidth, 1920);
    metaData.setInt32(kKeyHeight, 1080);
    metaData.setInt32(kKeyColorFormat, COLOR_FormatYUV420Planar);
    int ret = video_source_->start(&metaData);
    LOG(LS_INFO) << "start ret: " << ret;
  });
  return OK;
}

status_t VideoCapturer::Stop() {
  task_runner_->postTask([this]() { video_source_->stop(); });
  return OK;
}

status_t VideoCapturer::AddVideoSink(std::shared_ptr<VideoSink> sink) {
  task_runner_->postTask([this, sink]() {
    for (auto& s : video_sinks_) {
      if (s == sink) {
        return;
      }
    }

    video_sinks_.push_back(std::move(sink));
    return;
  });
  return OK;
}

status_t VideoCapturer::SetProcessor(
    std::shared_ptr<VideoProcessor> processor) {
  task_runner_->postTask([this, processor]() {
    if (processor != nullptr) {
      processor->SetVideoSink(nullptr);
    }
    if (processor != nullptr) {
      // video capturer lifecycle must longer than video processor
      video_processor_->SetVideoSink(this);
    }
  });
  return OK;
}

void VideoCapturer::OnFrameCaptured(std::shared_ptr<Buffer> buffer) {
  task_runner_->postTask([this, buffer]() {
    if (video_processor_ != nullptr) {
      video_processor_->OnProcess(buffer);
    } else {
      for (auto& sink : video_sinks_) {
        sink->OnFrame(buffer);
      }
    }
  });
}

uint64_t VideoCapturer::CaptureOneFrame() {
  std::shared_ptr<Buffer> buffer;
  status_t ret;

  ret = video_source_->read(buffer);
  if (buffer != nullptr && ret == OK) {
    OnFrameCaptured(buffer);
  }

  // 16ms, about 60fps. capture faster than real fps
  return 16 * 1000;
}

void VideoCapturer::OnSinkNotify(std::shared_ptr<Message> notify) {
  // check fps in notify
  int32_t fps;
  DCHECK(notify->findInt32("fps", &fps));
}

void VideoCapturer::OnFrame(std::shared_ptr<Buffer> buffer) {
  task_runner_->postTask([this, &buffer]() {
    for (auto& sink : video_sinks_) {
      sink->OnFrame(buffer);
    }
  });
}

void VideoCapturer::onMessageReceived(const std::shared_ptr<Message>& message) {
  switch (message->what()) {
    case kWhatSinkNotify: {
      OnSinkNotify(message);
      break;
    }
    default:
      break;
  }
}

}  // namespace avp
