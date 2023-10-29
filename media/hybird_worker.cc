/*
 * hybird_worker.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "hybird_worker.h"

#include <memory>

#include "api/audio/audio_frame.h"
#include "api/audio/audio_sink_interface.h"
#include "base/checks.h"
#include "base/sequence_checker.h"
#include "base/task_util/default_task_runner_factory.h"
#include "common/audio_codec_property.h"
#include "media/audio/audio_flinger.h"
#include "media/video/video_stream_sender.h"

namespace avp {

HybirdWorker::HybirdWorker(base::TaskRunnerFactory* task_factory,
                           AudioEncoderFactory* audio_encoder_factory,
                           VideoEncoderFactory* video_encoder_factory,
                           AudioDevice* audio_device)
    : MediaWorker(task_factory,
                  audio_encoder_factory,
                  video_encoder_factory,
                  audio_device),
      worker_task_runner_(task_runner_factory()->CreateTaskRunner(
          "HybirdWorkerRunner",
          base::TaskRunnerFactory::Priority::NORMAL)),
      audio_flinger_(audio_device),
      media_transport_(
          std::make_unique<MediaTransport>(task_runner_factory())) {
  audio_flinger_.InitIfNeed();
}

HybirdWorker::~HybirdWorker() {}

void HybirdWorker::AddVideoSource(VideoSource& video_source,
                                  int32_t stream_id,
                                  VideoEncoderConfig& encoder_config) {
  worker_task_runner_.PostTask([this, video_source, stream_id,
                                config = encoder_config.Copy()]() mutable {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);
    // warning if sources already exist in video_sources_
    auto it = std::find_if(video_sources_.begin(), video_sources_.end(),
                           [&video_source](const VideoSourceInfo& source) {
                             return source.video_source == video_source;
                           });
    if (it != video_sources_.end()) {
      LOG(LS_WARNING) << "VideoSource already exist in video_sources_";
      return;
    }

    // add video source to video_sources_
    video_sources_.push_back({video_source, stream_id});

    VideoStreamSender* stream_sender =
        media_transport_->GetVideoStreamSender(stream_id);

    // create video send stream
    video_send_streams_.push_back(
        {std::make_unique<VideoSendStream>(
             task_runner_factory(), &worker_task_runner_,
             video_encoder_factory(), video_source.get(), stream_sender,
             std::move(config)),
         stream_id});

    // start video send stream if media transport wants frame
    // if no sink exist, video send stream when VideiSink is added
    if (media_transport_->frame_wanted(stream_id)) {
      video_send_streams_.back().video_send_stream->Start();
    }
  });
}

void HybirdWorker::RemoveVideoSource(VideoSource& video_source,
                                     int32_t stream_id) {
  worker_task_runner_.PostTask([this, video_source, stream_id]() {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);
    auto it_video_source =
        std::find_if(video_sources_.begin(), video_sources_.end(),
                     [&video_source](const VideoSourceInfo& source) {
                       return source.video_source == video_source;
                     });
    if (it_video_source == video_sources_.end()) {
      LOG(LS_WARNING) << "VideoSource not exist in video_sources_";
      return;
    }

    DCHECK(it_video_source->stream_id == stream_id);

    // find, stop and erase video send stream
    auto it_send_stream =
        std::find_if(video_send_streams_.begin(), video_send_streams_.end(),
                     [&it_video_source](const VideoSendStreamInfo& stream) {
                       return stream.stream_id == it_video_source->stream_id;
                     });
    DCHECK(it_send_stream != video_send_streams_.end());

    it_send_stream->video_send_stream->Stop();
    video_send_streams_.erase(it_send_stream);

    // erase stream sender
    media_transport_->RemoveVideoStreamSender(it_video_source->stream_id);

    // remove video source from video_sources_
    video_sources_.erase(it_video_source);
  });
}

void HybirdWorker::AddEncodedVideoSink(EncodedVideoSink& encoded_image_sink,
                                       int32_t stream_id) {
  worker_task_runner_.PostTask([this, encoded_image_sink, stream_id]() {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);
    // already has sink means video send stream already started
    bool started = media_transport_->frame_wanted(stream_id);
    media_transport_->AddVideoSink(encoded_image_sink, stream_id);
    if (!started) {
      auto it_send_stream =
          std::find_if(video_send_streams_.begin(), video_send_streams_.end(),
                       [stream_id](const VideoSendStreamInfo& stream) {
                         return stream.stream_id == stream_id;
                       });

      if (it_send_stream != video_send_streams_.end()) {
        it_send_stream->video_send_stream->Start();
      }
    }
  });
}

void HybirdWorker::RemoveEncodedVideoSink(EncodedVideoSink& encoded_image_sink,
                                          int32_t stream_id) {
  worker_task_runner_.PostTask([this, encoded_image_sink, stream_id]() {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);
    media_transport_->RemoveVideoSink(encoded_image_sink);
    // stop video send stream if no sink exist
    if (!media_transport_->frame_wanted(stream_id)) {
      auto it_send_stream =
          std::find_if(video_send_streams_.begin(), video_send_streams_.end(),
                       [stream_id](const VideoSendStreamInfo& stream) {
                         return stream.stream_id == stream_id;
                       });
      if (it_send_stream != video_send_streams_.end()) {
        it_send_stream->video_send_stream->Stop();
      }
    }
  });
}

void HybirdWorker::RequestKeyFrame() {
  worker_task_runner_.PostTask([this]() {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);
    for (auto it = video_send_streams_.begin(); it != video_send_streams_.end();
         ++it) {
      it->video_send_stream->RequestKeyFrame();
    }
  });
}

void HybirdWorker::AddEncodedAudioSink(
    std::shared_ptr<AudioSinkInterface<MediaPacket>>& audio_sink,
    int32_t stream_id,
    CodecId codec_id) {
  worker_task_runner_.PostTask([this, audio_sink, stream_id, codec_id]() {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);

    auto send_stream_it =
        std::find_if(audio_send_streams_.begin(), audio_send_streams_.end(),
                     [codec_id](const AudioSendStreamInfo& stream) {
                       return stream.codec_id == codec_id;
                     });

    if (send_stream_it == audio_send_streams_.end()) {
      // audio send stream differs with codec id
      // new codec, create audio send stream

      AudioStreamSender* audio_stream_sender =
          media_transport_->GetAudioStreamSender(stream_id, codec_id);
      audio_send_streams_.push_back(
          {std::make_shared<AudioSendStream>(task_runner_factory(),
                                             audio_stream_sender,
                                             audio_encoder_factory()),
           stream_id, codec_id, 1});

      // TODO(youfa) hardcode now, refine later
      AudioCodecProperty codec_property;
      codec_property.codec_id = codec_id;
      codec_property.channels = 2;
      codec_property.samples_per_channel = 1024;
      codec_property.sample_rate = 44100;
      codec_property.bit_rate = 64000;

      audio_send_streams_.back().audio_send_stream->ConfigureEncoder(
          codec_property);

    } else {
      send_stream_it->sink_count++;
    }
    media_transport_->AddAudioSenderSink(audio_sink, stream_id, codec_id);
    UpdateAudioFlingerWithSenders();
  });
}

void HybirdWorker::RemoveEncodedAudioSink(
    std::shared_ptr<AudioSinkInterface<MediaPacket>>& audio_sink,
    CodecId codec_id) {
  worker_task_runner_.PostTask([this, audio_sink, codec_id]() {
    AVP_DCHECK_RUN_ON(&worker_task_runner_);
    auto send_stream_it =
        std::find_if(audio_send_streams_.begin(), audio_send_streams_.end(),
                     [codec_id](const AudioSendStreamInfo& stream) {
                       return stream.codec_id == codec_id;
                     });
    DCHECK(send_stream_it != audio_send_streams_.end());

    send_stream_it->sink_count--;
    media_transport_->RemoveAudioSenderSink(audio_sink);

    if (send_stream_it->sink_count == 0) {
      // no sink left, remove audio send stream
      media_transport_->RemoveAudioStreamSender(codec_id);
      audio_send_streams_.erase(send_stream_it);
    }
    UpdateAudioFlingerWithSenders();
  });
}

void HybirdWorker::AddAudioStreamReceiver() {}
void HybirdWorker::RemoveAudioStreamReceiver() {}

void HybirdWorker::UpdateAudioFlingerWithSenders() {
  LOG(LS_INFO) << "UpdateAudioFlingerWithSenders";
  std::vector<std::shared_ptr<AudioSendStream>> senders;
  for (auto& sender : audio_send_streams_) {
    senders.push_back(sender.audio_send_stream);
  }
  audio_flinger_.UpdateSender(senders);
}

}  // namespace avp
