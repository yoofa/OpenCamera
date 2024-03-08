/*
 * video_stream_encoder_interface.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_STREAM_ENCODER_INTERFACE_H
#define VIDEO_STREAM_ENCODER_INTERFACE_H

#include <memory>

#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_source_interface.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/video_encoder_config.h"

namespace ave {
class VideoStreamEncoderInterface
    : public VideoSinkInterface<std::shared_ptr<VideoFrame>> {
 public:
  // Interface for receiving encoded video frames and notifications about
  // configuration changes.
  class EncoderSink : public EncodedImageCallback {};

  virtual void SetSource(
      VideoSourceInterface<std::shared_ptr<VideoFrame>>* source) = 0;

  virtual void SetSink(EncoderSink* sink) = 0;

  virtual void SetStartBitrate(int start_bitrate_bps) = 0;

  // Request a key frame. Used for signalling from the remote receiver.
  virtual void SendKeyFrame() = 0;

  // Creates and configures an encoder with the given `config`. The
  // `max_data_payload_length` is used to support single NAL unit
  // packetization for H.264.
  virtual void ConfigureEncoder(VideoEncoderConfig config,
                                size_t max_data_payload_length) = 0;

  // Permanently stop encoding. After this method has returned, it is
  // guaranteed that no encoded frames will be delivered to the sink.
  virtual void Stop() = 0;
};
}  // namespace ave

#endif /* !VIDEO_STREAM_ENCODER_INTERFACE_H */
