/*
 * audio_stream_sender.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_stream_sender.h"

#include "base/logging.h"
#include "base/sequence_checker.h"
#include "base/task_util/task_runner.h"

namespace ave {

AudioStreamSender::AudioStreamSender(base::TaskRunner* transport_runner)
    : transport_runner_(transport_runner) {}

AudioStreamSender::~AudioStreamSender() {}

void AudioStreamSender::OnFrame(const MediaPacket packet) {
  transport_runner_->PostTask([this, packet = std::move(packet)]() {
    AVE_DCHECK_RUN_ON(transport_runner_);

    for (auto& sink : sinks_) {
      sink->OnFrame(packet);
    }
  });
}

void AudioStreamSender::AddAudioSink(const std::shared_ptr<AudioSinkT> sink) {
  transport_runner_->PostTask([this, sink = std::move(sink)]() {
    AVE_DCHECK_RUN_ON(transport_runner_);

    auto it = std::find_if(sinks_.begin(), sinks_.end(),
                           [&sink](const std::shared_ptr<AudioSinkT>& s) {
                             return s.get() == sink.get();
                           });

    if (it != sinks_.end()) {
      AVE_LOG(LS_WARNING)
          << "AudioStreamSender::AddAudioSink() sink already exists";
      return;
    }

    sinks_.push_back(sink);
    AVE_LOG(LS_INFO) << "AudioStreamSender::AddAudioSink()";
  });
}

void AudioStreamSender::RemoveAudioSink(
    const std::shared_ptr<AudioSinkT> sink) {
  transport_runner_->PostTask([this, sink = std::move(sink)]() {
    AVE_DCHECK_RUN_ON(transport_runner_);

    auto it = std::find_if(sinks_.begin(), sinks_.end(),
                           [&sink](const std::shared_ptr<AudioSinkT>& s) {
                             return s.get() == sink.get();
                           });

    if (it == sinks_.end()) {
      AVE_LOG(LS_WARNING)
          << "AudioStreamSender::RemoveAudioSink() sink does not exist";
      return;
    }

    sinks_.erase(it);
    AVE_LOG(LS_INFO) << "AudioStreamSender::RemoveAudioSink()";
  });
}

}  // namespace ave
