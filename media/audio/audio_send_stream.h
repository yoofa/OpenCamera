/*
 * audio_send_stream.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_SEND_STREAM_H
#define AUDIO_SEND_STREAM_H

#include <memory>

#include "api/audio/audio_frame.h"
#include "api/audio_codecs/audio_encoder.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "media/audio/audio_stream_sender.h"

namespace avp {
class AudioSendStream : public AudioEncoder::EncodedCallback {
 public:
  AudioSendStream(base::TaskRunnerFactory* task_runner_factory_,
                  AudioStreamSender* audio_stream_sender,
                  AudioEncoderFactory* audio_encoder_factory);
  virtual ~AudioSendStream();
  void ConfigureEncoder(const AudioCodecProperty& codec_settings);

  void SendAudioData(std::shared_ptr<AudioFrame> audio_frame);

  void OnEncoded(std::shared_ptr<Buffer8> frame) override;

  size_t samples_per_channel() const { return samples_per_channel_; }
  int sample_rate_hz() const { return sample_rate_hz_; }
  size_t num_channels() const { return num_channels_; }

 private:
  void MaybeEncodeAudioFrame(const std::shared_ptr<AudioFrame>& audio_frame);
  void ReconfigureEncoder();
  void EncodeAudioFrame(const std::shared_ptr<AudioFrame>& audio_frame);
  base::TaskRunnerFactory* task_runner_factory_;
  base::TaskRunner task_runner_;
  AudioStreamSender* audio_stream_sender_;
  AudioEncoderFactory* audio_encoder_factory_;
  std::unique_ptr<AudioEncoder> audio_encoder_;
  CodecId codec_id_;
  size_t samples_per_channel_;
  int sample_rate_hz_;
  size_t num_channels_;
  uint32_t bit_rate_;
  bool pending_reconfigure_encoder_;
};
}  // namespace avp

#endif /* !AUDIO_SEND_STREAM_H */
