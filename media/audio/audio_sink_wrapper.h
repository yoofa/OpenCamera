/*
 * audio_sink_wrapper.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_SINK_WRAPPER_H
#define AUDIO_SINK_WRAPPER_H

#include <memory>
#include "api/audio/audio_frame.h"
#include "api/audio/audio_sink_interface.h"
#include "common/message.h"

namespace ave {

template <typename AudioFrameT>
class AudioSinkWrapper : public AudioSinkInterface<AudioFrameT>,
                         public MessageObject {
 protected:
  // for private construct
  struct protect_parameter {
    explicit protect_parameter() {}
  };

 public:
  static auto Create(std::shared_ptr<AudioSinkInterface<AudioFrameT>> sink) {
    return std::make_shared<AudioSinkWrapper<AudioFrameT>>(sink,
                                                           protect_parameter{});
  }

  uint32_t sample_rate() const override { return sink_->sample_rate(); }
  uint16_t channel_count() const override { return sink_->channel_count(); }

  void OnFrame(const AudioFrameT frame) override {
    sink_->OnFrame(std::move(frame));
  }

  explicit AudioSinkWrapper(
      std::shared_ptr<AudioSinkInterface<AudioFrameT>> sink,
      protect_parameter) {
    sink_ = std::move(sink);
  }

  const std::shared_ptr<AudioSinkInterface<AudioFrameT>>& sink() const {
    return sink_;
  }

 private:
  std::shared_ptr<AudioSinkInterface<AudioFrameT>> sink_;
};

}  // namespace ave

#endif /* !AUDIO_SINK_WRAPPER_H */
