/*
 * audio_sink_interface.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_SINK_INTERFACE_H
#define AUDIO_SINK_INTERFACE_H
#include "base/types.h"

namespace avp {

template <typename AudioFrame>
class AudioSinkInterface {
 public:
  virtual ~AudioSinkInterface() = default;

  // sink own the reference explicitly
  virtual void OnFrame(const AudioFrame frame) = 0;

  virtual uint32_t sample_rate() const { return sample_rate_; }
  virtual uint16_t channel_count() const { return channel_count_; }

 protected:
  void set_sample_rate(uint32_t sample_rate) { sample_rate_ = sample_rate; }
  void set_channel_count(uint16_t channel_count) {
    channel_count_ = channel_count;
  }

 private:
  uint32_t sample_rate_ = 0;
  uint16_t channel_count_ = 0;
};
}  // namespace avp

#endif /* !AUDIO_SINK_INTERFACE_H */
