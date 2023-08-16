/*
 * openh264_encoder.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AVP_VIDEO_CODECS_OPENH264_ENCODER_H
#define AVP_VIDEO_CODECS_OPENH264_ENCODER_H

#include "common/video_codec_property.h"
#include "modules/video_coding/codecs/h264/h264.h"
#include "third_party/openh264/src/codec/api/svc/codec_api.h"

namespace avp {

class OpenH264Encoder : public H264Encoder {
 public:
  OpenH264Encoder();
  virtual ~OpenH264Encoder();

  status_t InitEncoder(const VideoCodecProperty& codec_property,
                       const Settings& encoder_settings) override;

  status_t Release() override;

  status_t RegisterEncoderCompleteCallback(
      EncodedImageCallback* callback) override;

  status_t Encode(const std::shared_ptr<VideoFrame>& frame) override;

  void RequestKeyFrame() override;

 private:
  struct LayerConfig {
    int simulcast_idx = 0;
    int width = -1;
    int height = -1;
    bool sending = true;
    bool key_frame_request = false;
    float max_frame_rate = 0;
    uint32_t target_bps = 0;
    uint32_t max_bps = 0;
    bool frame_dropping_on = false;
    int key_frame_interval = 0;
    int num_temporal_layers = 1;

    void SetStreamState(bool send_stream);
  };

  SEncParamExt CreateEncoderParams() const;
  void PacketizeEncodedImage(EncodedImage* encoded_image, SFrameBSInfo* info);

  size_t max_payload_size_;
  int32_t number_of_cores_;
  VideoCodecProperty codec_property_;

  ISVCEncoder* encoder_;
  SSourcePicture picture_;
  // rtc::scoped_refptr<I420Buffer>> downscaled_buffers_;
  LayerConfig configuration_;
  EncodedImage encoded_image_;

  EncodedImageCallback* encoded_image_callback_;
};

}  // namespace avp

#endif /* !AVP_VIDEO_CODECS_OPENH264_ENCODER_H */
