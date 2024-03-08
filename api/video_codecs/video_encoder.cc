/*
 * video_encoder.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_encoder.h"

#include "base/types.h"
#include "common/video_codec_property.h"

namespace ave {

H264Specific VideoEncoder::GetDefaultH264Specific() {
  H264Specific h264_specific;
  memset(&h264_specific, 0, sizeof(h264_specific));
  h264_specific.number_of_temporal_layers = 1;
  h264_specific.frame_dropping_on = true;
  h264_specific.key_frame_interval = 3000;

  return h264_specific;
}
H265Specific VideoEncoder::GetDefaultH265Specific() {
  H265Specific h265_specific;
  return h265_specific;
}
VP8Specific VideoEncoder::GetDefaultVp8Specific() {
  VP8Specific vp8_specific;
  memset(&vp8_specific, 0, sizeof(vp8_specific));
  vp8_specific.number_of_temporal_layers = 1;
  vp8_specific.denoising_on = true;
  vp8_specific.automatic_resize_on = false;
  vp8_specific.frame_dropping_on = true;
  vp8_specific.key_frame_interval = 3000;
  return vp8_specific;
}

VP9Specific VideoEncoder::GetDefaultVp9Specific() {
  VP9Specific vp9_specific;
  memset(&vp9_specific, 0, sizeof(vp9_specific));
  vp9_specific.number_of_temporal_layers = 1;
  vp9_specific.denoising_on = true;
  vp9_specific.frame_dropping_on = true;
  vp9_specific.key_frame_interval = 3000;
  vp9_specific.adaptive_qp_mode = true;
  vp9_specific.automatic_resize_on = false;
  vp9_specific.number_of_spatial_layers = 1;
  vp9_specific.flexible_mode = false;
  vp9_specific.inter_layer_pred = InterLayerPredMode::kOn;
  return vp9_specific;
}

status_t VideoEncoder::InitEncoder(const VideoCodecProperty& codec_settings,
                                   const Settings& settings) {
  return OK;
}
}  // namespace ave
