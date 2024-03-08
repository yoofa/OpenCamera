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

namespace ave {

VideoCapturer::VideoCapturer(std::shared_ptr<Message> capture_info)
    : capture_info_(std::move(capture_info)),
      task_runner_factory_(base::CreateDefaultTaskRunnerFactory()),
      task_runner_(std::make_unique<base::TaskRunner>(
          task_runner_factory_->CreateTaskRunner(
              "VideoCapturer",
              base::TaskRunnerFactory::Priority::NORMAL))),
      video_source_(nullptr),
      video_processor_(nullptr),
      frame_received_(0),
      frame_sent_(0) {
  // ctor
}

VideoCapturer::~VideoCapturer() {}

void VideoCapturer::AddOrUpdateSinkInternal(VideoSink* sink,
                                            const VideoSinkWants& wants) {
  sinks_broadcaster_.AddOrUpdateSink(sink, wants);
}

void VideoCapturer::AddOrUpdateSink(VideoSink* sink,
                                    const VideoSinkWants& wants) {
  task_runner_->PostTask(
      [this, sink, wants]() { AddOrUpdateSinkInternal(sink, wants); });
}

void VideoCapturer::RemoveSink(VideoSink* sink) {
  task_runner_->PostTask(
      [this, sink]() { sinks_broadcaster_.RemoveSink(sink); });
}

void VideoCapturer::OnFrame(const std::shared_ptr<VideoFrame>& frame) {
  task_runner_->PostTask([this, frame]() {
    frame_received_++;
    // TODO(youfa) frame rate control
    // if (drop) {
    //   return;
    // }

    if (video_processor_ != nullptr) {
      // after process, processor will call OnProcessedFrame
      video_processor_->OnFrame(frame);
    } else {
      if (sinks_broadcaster_.sink_pairs().empty()) {
        return;
      }
      frame_sent_++;
      for (auto& sink : sinks_broadcaster_.sink_pairs()) {
        sink.sink->OnFrame(frame);
      }
    }
  });
}

void VideoCapturer::OnProcessedFrame(std::shared_ptr<VideoFrame>& frame) {
  task_runner_->PostTask([this, frame]() {
    frame_sent_++;
    for (auto& sink : sinks_broadcaster_.sink_pairs()) {
      sink.sink->OnFrame(frame);
    }
  });
}

void VideoCapturer::SetVideoSource(VideoSource* video_source,
                                   const VideoSinkWants& wants) {
  task_runner_->PostTask([this, video_source, wants]() {
    if (video_source_ != nullptr) {
      video_source_->RemoveSink(this);
    }
    video_source_ = video_source;
    if (video_source_ != nullptr) {
      video_source_->AddOrUpdateSink(this, wants);
    }
  });
}

status_t VideoCapturer::SetProcessor(
    std::shared_ptr<VideoProcessor>& processor) {
  task_runner_->PostTask([this, processor]() {
    if (video_processor_ != nullptr) {
      video_processor_->RemoveSink(this);
    }
    video_processor_ = processor;
    if (video_processor_ != nullptr) {
      video_processor_->AddOrUpdateSink(this, VideoSinkWants{});
    }
  });
  return OK;
}

}  // namespace ave
