/*
 * audio_send_stream.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "audio_send_stream.h"

#include "base/logging.h"
#include "base/sequence_checker.h"
#include "common/audio_codec_property.h"

namespace ave {

AudioSendStream::AudioSendStream(base::TaskRunnerFactory* task_runner_factory,
                                 AudioStreamSender* audio_stream_sender,
                                 AudioEncoderFactory* audio_encoder_factory)
    : task_runner_factory_(task_runner_factory),
      task_runner_(task_runner_factory_->CreateTaskRunner(
          "AudioSendStream",
          base::TaskRunnerFactory::Priority::HIGH)),
      audio_stream_sender_(audio_stream_sender),
      audio_encoder_factory_(audio_encoder_factory),
      samples_per_channel_(0),
      sample_rate_hz_(0),
      num_channels_(0) {}

AudioSendStream::~AudioSendStream() {}

void AudioSendStream::ConfigureEncoder(
    const AudioCodecProperty& codec_settings) {
  codec_id_ = codec_settings.codec_id;
  samples_per_channel_ = codec_settings.samples_per_channel;
  sample_rate_hz_ = codec_settings.sample_rate;
  num_channels_ = codec_settings.channels;
  bit_rate_ = codec_settings.bit_rate;
}

void AudioSendStream::SendAudioData(std::shared_ptr<AudioFrame> audio_frame) {
  AVE_LOG(LS_VERBOSE) << "SendAudioData";
  task_runner_.PostTask([this, audio_frame = std::move(audio_frame)]() {
    AVE_DCHECK_RUN_ON(&task_runner_);
    MaybeEncodeAudioFrame(audio_frame);
  });
}

void AudioSendStream::OnEncoded(const MediaPacket packet) {
  AVE_LOG(LS_VERBOSE) << "OnEncoded";
  task_runner_.PostTask([this, packet = std::move(packet)]() {
    AVE_DCHECK_RUN_ON(&task_runner_);
    audio_stream_sender_->OnFrame(std::move(packet));
  });
}

void AudioSendStream::MaybeEncodeAudioFrame(
    const std::shared_ptr<AudioFrame>& audio_frame) {
  AVE_DCHECK_RUN_ON(&task_runner_);
  if (samples_per_channel_ != audio_frame->samples_per_channel() ||
      sample_rate_hz_ != audio_frame->sample_rate_hz() ||
      num_channels_ != audio_frame->num_channels()) {
    AVE_LOG(LS_INFO)
        << "audio frame changed, (samples_per_channel, sample_rate_hz, "
           "num_channels) = ("
        << audio_frame->samples_per_channel() << ", "
        << audio_frame->sample_rate_hz() << ", " << audio_frame->num_channels()
        << "))";

    samples_per_channel_ = audio_frame->samples_per_channel();
    sample_rate_hz_ = audio_frame->sample_rate_hz();
    num_channels_ = audio_frame->num_channels();
    pending_reconfigure_encoder_ = true;
  }
  if (pending_reconfigure_encoder_) {
    ReconfigureEncoder();
    pending_reconfigure_encoder_ = false;
  }
  EncodeAudioFrame(audio_frame);
}

void AudioSendStream::ReconfigureEncoder() {
  AVE_LOG(LS_VERBOSE) << "ReconfigureEncoder";
  AVE_DCHECK_RUN_ON(&task_runner_);
  AVE_DCHECK(pending_reconfigure_encoder_);
  if (!audio_encoder_) {
    audio_encoder_ = audio_encoder_factory_->CreateAudioEncoder(codec_id_);
  }
  AVE_DCHECK(audio_encoder_);
  AudioCodecProperty codec_settings;
  codec_settings.codec_id = codec_id_;
  codec_settings.samples_per_channel = samples_per_channel_;
  codec_settings.sample_rate = sample_rate_hz_;
  codec_settings.channels = num_channels_;
  codec_settings.bit_rate = bit_rate_;
  audio_encoder_->InitEncoder(codec_settings);
  audio_encoder_->RegisterEncoderCompleteCallback(this);
}

void AudioSendStream::EncodeAudioFrame(
    const std::shared_ptr<AudioFrame>& audio_frame) {
  AVE_LOG(LS_VERBOSE) << "EncodeAudioFrame";
  AVE_DCHECK_RUN_ON(&task_runner_);
  if (!audio_encoder_) {
    AVE_LOG(LS_ERROR) << "No audio encoder.";
    return;
  }
  status_t ret = audio_encoder_->Encode(audio_frame);
  if (ret < 0) {
    AVE_LOG(LS_ERROR) << "Encode failed: " << ret;
  }
}

}  // namespace ave
