/*
 * video_encoder.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_ENCODER_H
#define VIDEO_ENCODER_H

#include <memory>

#include "api/video/encoded_image.h"
#include "api/video/video_frame.h"
#include "common/video_codec_property.h"

namespace avp {

class EncodedImageCallback {
 public:
  virtual ~EncodedImageCallback() = default;

  struct Result {
    enum Error {
      OK,

      // Failed to send the packet.
      ERROR_SEND_FAILED,
    };

    explicit Result(Error error) : error(error) {}
    Result(Error error, uint32_t frame_id) : error(error), frame_id(frame_id) {}

    Error error;

    // Frame ID assigned to the frame. The frame ID should be the same as the ID
    // seen by the receiver for this frame. RTP timestamp of the frame is used
    // as frame ID when RTP is used to send video. Must be used only when
    // error=OK.
    uint32_t frame_id = 0;

    // Tells the encoder that the next frame is should be dropped.
    bool drop_next_frame = false;
  };

  // Used to signal the encoder about reason a frame is dropped.
  // kDroppedByMediaOptimizations - dropped by MediaOptimizations (for rate
  // limiting purposes).
  // kDroppedByEncoder - dropped by encoder's internal rate limiter.
  enum class DropReason : uint8_t {
    kDroppedByMediaOptimizations,
    kDroppedByEncoder
  };

  virtual Result OnEncodedImage(const std::shared_ptr<EncodedImage>& frame) = 0;

  virtual void OnDroppedFrame(DropReason reason) {}
};

// Negotiated capabilities which the VideoEncoder may expect the other
// side to use.
struct Capabilities {
  explicit Capabilities(bool loss_notification)
      : loss_notification(loss_notification) {}
  bool loss_notification;
};

struct Settings {
  Settings(const Capabilities& capabilities,
           int number_of_cores,
           size_t max_payload_size)
      : capabilities(capabilities),
        number_of_cores(number_of_cores),
        max_payload_size(max_payload_size) {}

  Capabilities capabilities;
  int number_of_cores;
  size_t max_payload_size;
};

class VideoEncoder {
 public:
  virtual ~VideoEncoder() = default;

  virtual status_t InitEncoder(const VideoCodecProperty& codec_settings,
                               const Settings& encoder_settings);

  virtual status_t RegisterEncodedImageCallback(
      EncodedImageCallback* callback) = 0;

  virtual status_t Release() = 0;

  virtual status_t Encode(const std::shared_ptr<VideoFrame>& frame) = 0;
};
}  // namespace avp

#endif /* !VIDEO_ENCODER_H */
