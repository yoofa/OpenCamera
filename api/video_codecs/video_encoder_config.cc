/*
 * video_encoder_config.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_encoder_config.h"

#include <sstream>
#include <string>
#include "common/video_codec_property.h"

#include "base/checks.h"

namespace avp {

VideoStreamConfig::VideoStreamConfig()
    : width(0),
      height(0),
      max_framerate(-1),
      min_bitrate_kbps(-1),
      target_bitrate_kbps(-1),
      max_bitrate_kbps(-1),
      scale_resolution_down_by(-1.),
      max_qp(-1),
      num_temporal_layers(std::nullopt),
      active(true) {}
VideoStreamConfig::VideoStreamConfig(const VideoStreamConfig& other) = default;

VideoStreamConfig::~VideoStreamConfig() = default;

std::string VideoStreamConfig::ToString() const {
  std::stringstream ss;
  ss << "{width: " << width;
  ss << ", height: " << height;
  ss << ", max_framerate: " << max_framerate;
  ss << ", min_bitrate_kbps:" << min_bitrate_kbps;
  ss << ", target_bitrate_kbps:" << target_bitrate_kbps;
  ss << ", max_bitrate_kbps:" << max_bitrate_kbps;
  ss << ", max_qp: " << max_qp;
  ss << ", num_temporal_layers: " << num_temporal_layers.value_or(1);
  ss << ", bitrate_priority: " << bitrate_priority.value_or(0);
  ss << ", active: " << active;
  ss << ", scale_down_by: " << scale_resolution_down_by;

  return ss.str();
}

VideoEncoderConfig::VideoEncoderConfig()
    : codec_id(CodecId::AV_CODEC_ID_NONE),
      //     video_format("Unset"),
      //     content_type(ContentType::kRealtimeVideo),
      //     encoder_specific_settings(nullptr),
      min_bitrate_kbps(0),
      max_bitrate_kbps(0),
      bitrate_priority(1.0),
      number_of_streams(0),
      is_quality_scaling_allowed(false) {}

VideoEncoderConfig::VideoEncoderConfig(VideoEncoderConfig&&) = default;

VideoEncoderConfig::~VideoEncoderConfig() = default;

std::string VideoEncoderConfig::ToString() const {
  std::stringstream ss;
  ss << "{codec_id: ";
  ss << CodecName(codec_id);
  // ss << ", content_type: ";
  //   switch (content_type) {
  //     case ContentType::kRealtimeVideo:
  //       ss << "kRealtimeVideo";
  //       break;
  //     case ContentType::kScreen:
  //       ss << "kScreenshare";
  //       break;
  //   }
  // ss << ", encoder_specific_settings: ";
  //  ss << (encoder_specific_settings != NULL ? "(ptr)" : "NULL");

  ss << ", min_bitrate_bps: " << min_bitrate_kbps;
  ss << ", max_bitrate_bps: " << max_bitrate_kbps;
  ss << '}';
  return ss.str();
}

VideoEncoderConfig::VideoEncoderConfig(const VideoEncoderConfig&) = default;

void VideoEncoderConfig::EncoderSpecificSettings::FillEncoderSpecificSettings(
    VideoCodecProperty* codec) const {
  if (codec->codec_id == CodecId::AV_CODEC_ID_H264) {
    FillVideoCodecH264(codec->H264());
  } else if (codec->codec_id == CodecId::AV_CODEC_ID_VP8) {
    FillVideoCodecVp8(codec->VP8());
  } else if (codec->codec_id == CodecId::AV_CODEC_ID_VP9) {
    FillVideoCodecVp9(codec->VP9());
  } else {
    AVP_NOTREACHED() << "Encoder specifics set/used for unknown codec type.";
  }
}

void VideoEncoderConfig::EncoderSpecificSettings::FillVideoCodecH264(
    H264Specific* h264_settings) const {
  AVP_NOTREACHED();
}

void VideoEncoderConfig::EncoderSpecificSettings::FillVideoCodecVp8(
    VP8Specific* vp8_settings) const {
  AVP_NOTREACHED();
}

void VideoEncoderConfig::EncoderSpecificSettings::FillVideoCodecVp9(
    VP9Specific* vp9_settings) const {
  AVP_NOTREACHED();
}

VideoEncoderConfig::H264EncoderSpecificSettings::H264EncoderSpecificSettings(
    const H264Specific& specifics)
    : specifics_(specifics) {}

void VideoEncoderConfig::H264EncoderSpecificSettings::FillVideoCodecH264(
    H264Specific* h264_settings) const {
  *h264_settings = specifics_;
}

VideoEncoderConfig::Vp8EncoderSpecificSettings::Vp8EncoderSpecificSettings(
    const VP8Specific& specifics)
    : specifics_(specifics) {}

void VideoEncoderConfig::Vp8EncoderSpecificSettings::FillVideoCodecVp8(
    VP8Specific* vp8_settings) const {
  *vp8_settings = specifics_;
}

VideoEncoderConfig::Vp9EncoderSpecificSettings::Vp9EncoderSpecificSettings(
    const VP9Specific& specifics)
    : specifics_(specifics) {}

void VideoEncoderConfig::Vp9EncoderSpecificSettings::FillVideoCodecVp9(
    VP9Specific* vp9_settings) const {
  *vp9_settings = specifics_;
}

}  // namespace avp
