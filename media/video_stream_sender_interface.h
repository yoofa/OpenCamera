/*
 * video_stream_sender_interface.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_STREAM_SENDER_INTERFACE_H
#define VIDEO_STREAM_SENDER_INTERFACE_H

#include "api/video_codecs/video_encoder.h"

namespace avp {

class VideoStreamSenderInterface : public EncodedImageCallback {
 public:
  virtual Result OnEncodedImage(
      const std::shared_ptr<EncodedImage>& encoded_image) = 0;
};

}  // namespace avp

#endif /* !VIDEO_STREAM_SENDER_INTERFACE_H */
