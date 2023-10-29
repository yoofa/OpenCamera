/*
 * audio_stream_sender.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_STREAM_SENDER_H
#define AUDIO_STREAM_SENDER_H

#include <memory>
#include <vector>

#include "api/audio/audio_frame.h"
#include "api/audio/audio_sink_interface.h"
#include "base/task_util/task_runner.h"
#include "base/thread_annotation.h"
#include "common/media_packet.h"

namespace avp {
class AudioStreamSender : public AudioSinkInterface<MediaPacket> {
 public:
  using AudioBufferT = MediaPacket;
  using AudioSinkT = AudioSinkInterface<MediaPacket>;

  AudioStreamSender(base::TaskRunner* transport_runner);

  ~AudioStreamSender();
  void OnFrame(const MediaPacket packet) override;
  // AudioSinkInterface implementation.

  void AddAudioSink(const std::shared_ptr<AudioSinkT> sink);
  void RemoveAudioSink(const std::shared_ptr<AudioSinkT> sink);

 private:
  base::TaskRunner* transport_runner_;
  std::vector<std::shared_ptr<AudioSinkT>> sinks_ GUARDED_BY(transport_runner_);
};
}  // namespace avp

#endif /* !AUDIO_STREAM_SENDER_H */
