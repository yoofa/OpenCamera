/*
 * audio_sink_interface.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_SINK_INTERFACE_H
#define AUDIO_SINK_INTERFACE_H

namespace avp {

template <typename AudioFrame>
class AudioSinkInterface {
 public:
  virtual ~AudioSinkInterface() = default;

  virtual void OnFrame(const AudioFrame& frame) = 0;
};
}  // namespace avp

#endif /* !AUDIO_SINK_INTERFACE_H */
