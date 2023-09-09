/*
 * media_transport.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "media_transport.h"
#include "base/task_util/task_runner_factory.h"
#include "media/audio/audio_stream_sender.h"

namespace avp {

MediaTransport::MediaTransport(base::TaskRunnerFactory* task_runner_factory)
    : transport_runner_(task_runner_factory->CreateTaskRunner(
          "TransportRunner",
          base::TaskRunnerFactory::Priority::NORMAL)) {}

MediaTransport::~MediaTransport() {}

VideoStreamSender* MediaTransport::GetVideoStreamSender(int32_t id) {
  auto it = std::find_if(
      video_stream_senders_.begin(), video_stream_senders_.end(),
      [id](const VideoStreamSenderInfo& info) { return info.id == id; });
  if (it != video_stream_senders_.end()) {
    return it->video_stream_sender.get();
  }
  video_stream_senders_.push_back(VideoStreamSenderInfo{
      std::make_unique<VideoStreamSender>(&transport_runner_), id});

  return video_stream_senders_.back().video_stream_sender.get();
}

void MediaTransport::RemoveVideoStreamSender(int32_t id) {
  // remove video_stream_sender from video_stream_senders_
  auto it = std::find_if(
      video_stream_senders_.begin(), video_stream_senders_.end(),
      [id](const VideoStreamSenderInfo& info) { return info.id == id; });
  if (it == video_stream_senders_.end()) {
    LOG(LS_WARNING)
        << "RemoveVideoStreamSender failed, video_stream_sender not found";

    return;
  }
  video_stream_senders_.erase(it);
}

void MediaTransport::AddVideoSink(const EncodedVideoSink& sink, int32_t id) {
  auto video_stream_sender = GetVideoStreamSender(id);
  video_stream_sender->AddVideoSink(sink);
}

void MediaTransport::RemoveVideoSink(const EncodedVideoSink& sink) {
  for (auto& video_stream_sender : video_stream_senders_) {
    video_stream_sender.video_stream_sender->RemoveVideoSink(sink);
  }
}

bool MediaTransport::frame_wanted(int32_t stream_id) const {
  for (auto& video_stream_sender : video_stream_senders_) {
    if (video_stream_sender.id == stream_id) {
      return video_stream_sender.video_stream_sender->frame_wanted();
    }
  }
  return false;
}

AudioStreamSender* MediaTransport::GetAudioStreamSender(int32_t stream_id,
                                                        CodecId codec_id) {
  auto it =
      std::find_if(audio_stream_senders_.begin(), audio_stream_senders_.end(),
                   [codec_id](const AudioStreamSenderInfo& info) {
                     return info.codec_id == codec_id;
                   });
  if (it != audio_stream_senders_.end()) {
    return it->audio_stream_sender.get();
  }
  audio_stream_senders_.push_back(
      {std::make_unique<AudioStreamSender>(&transport_runner_), stream_id,
       codec_id});
  return audio_stream_senders_.back().audio_stream_sender.get();
}

void MediaTransport::RemoveAudioStreamSender(CodecId codec_id) {
  auto it =
      std::find_if(audio_stream_senders_.begin(), audio_stream_senders_.end(),
                   [codec_id](const AudioStreamSenderInfo& info) {
                     return info.codec_id == codec_id;
                   });
  if (it == audio_stream_senders_.end()) {
    LOG(LS_WARNING)
        << "RemoveAudioStreamSender failed, audio_stream_sender not found";

    return;
  }
  audio_stream_senders_.erase(it);
}

void MediaTransport::AddAudioSenderSink(
    const std::shared_ptr<AudioSinkInterface<std::shared_ptr<AudioFrame>>>&
        sink,
    int32_t stream_id,
    CodecId codec_id) {
  auto audio_stream_sender = GetAudioStreamSender(stream_id, codec_id);
  audio_stream_sender->AddAudioSink(sink);
}

void MediaTransport::RemoveAudioSenderSink(
    const std::shared_ptr<AudioSinkInterface<std::shared_ptr<AudioFrame>>>&
        sink) {
  // TODO(youfa) not all stream sender has this sink
  for (auto& audio_stream_sender : audio_stream_senders_) {
    audio_stream_sender.audio_stream_sender->RemoveAudioSink(sink);
  }
}

}  // namespace avp
