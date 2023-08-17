/*
 * video_codec_properity_initializer.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "video_stream_helper.h"

#include "api/video_codecs/video_encoder.h"
#include "base/checks.h"
#include "base/logging.h"
#include "base/task_util/default_task_runner_factory.h"
#include "common/codec_id.h"

namespace avp {
namespace {
static const uint32_t kEncoderMinBitrateKbps = 30;
static const int kDefaultVideoMaxFramerate = 60;

}  // namespace

// The selected thresholds for QVGA and VGA corresponded to a QP around 10.
// The change in QP declined above the selected bitrates.
static int32_t GetMaxDefaultVideoBitrateKbps(int width,
                                             int height,
                                             bool is_screenshare) {
  int max_bitrate;
  if (width * height <= 320 * 240) {
    max_bitrate = 600;
  } else if (width * height <= 640 * 480) {
    max_bitrate = 1700;
  } else if (width * height <= 960 * 540) {
    max_bitrate = 2000;
  } else {
    max_bitrate = 2500;
  }
  if (is_screenshare)
    max_bitrate = std::max(max_bitrate, 1200);
  return max_bitrate;
}

std::vector<VideoStreamConfig> CreateDefaultVideoStreamConfig(
    size_t width,
    size_t height,
    const VideoEncoderConfig& video_encoder_config) {
  std::vector<VideoStreamConfig> video_stream_configs;

  int32_t max_bitrate_kbps =
      video_encoder_config.max_bitrate_kbps > 0
          ? video_encoder_config.max_bitrate_kbps
          : GetMaxDefaultVideoBitrateKbps(width, height, false);

  int32_t min_bitrate_kbps = video_encoder_config.min_bitrate_kbps > 0
                                 ? video_encoder_config.min_bitrate_kbps
                                 : kEncoderMinBitrateKbps;

  VideoStreamConfig stream_config;
  stream_config.width = width;
  stream_config.height = height;

  // TODO(youfa) frame rate and qp
  // stream_config.max_framerate =
  // stream_config.qp_max = 56;

  stream_config.min_bitrate_kbps = std::min(min_bitrate_kbps, max_bitrate_kbps);
  stream_config.target_bitrate_kbps = max_bitrate_kbps;
  stream_config.max_bitrate_kbps = max_bitrate_kbps;

  video_stream_configs.push_back(stream_config);
  return video_stream_configs;
}

std::vector<VideoStreamConfig> CreateVideoStreamConfig(
    size_t width,
    size_t height,
    const VideoEncoderConfig& video_encoder_config) {
  // TODO(youfa) return simulcast stream config

  return CreateDefaultVideoStreamConfig(width, height, video_encoder_config);
}

bool SetupVideoCodecProperity(
    const VideoEncoderConfig& video_encoder_config,
    const std::vector<VideoStreamConfig>& stream_configs,
    VideoCodecProperty* codec_properity) {
  LOG(LS_INFO) << "SetupVideoCodecProperity";
  if (!codec_properity) {
    return false;
  }

  DCHECK_GT(video_encoder_config.min_bitrate_kbps, 0);

  codec_properity->codec_id = video_encoder_config.codec_id;
  codec_properity->mode = VideoCodecMode::kRealtimeVideo;

  codec_properity->bit_rate_range.min =
      std::max(stream_configs[0].min_bitrate_kbps, kEncoderMinBitrateKbps);

  int max_framerate = kDefaultVideoMaxFramerate;

  for (size_t i = 0; i < stream_configs.size(); i++) {
    codec_properity->width =
        std::max(codec_properity->width, stream_configs[i].width);
    codec_properity->height =
        std::max(codec_properity->height, stream_configs[i].height);
    codec_properity->bit_rate_range.min =
        std::max(codec_properity->bit_rate_range.min,
                 stream_configs[i].min_bitrate_kbps);
    codec_properity->bit_rate_range.max += stream_configs[i].max_bitrate_kbps;
    codec_properity->qp_range.max =
        std::max(codec_properity->qp_range.max, stream_configs[i].max_qp);
    max_framerate = std::max(max_framerate, stream_configs[i].max_framerate);
  }

  if (codec_properity->bit_rate_range.max == 0) {
    codec_properity->bit_rate_range.max = GetMaxDefaultVideoBitrateKbps(
        codec_properity->width, codec_properity->height, false);
  }

  codec_properity->bit_rate_range.max =
      std::max(codec_properity->bit_rate_range.max, kEncoderMinBitrateKbps);

  codec_properity->bit_rate = codec_properity->bit_rate_range.max;
  LOG(LS_INFO) << "bitrate:" << codec_properity->bit_rate;

  codec_properity->frame_rate = max_framerate;

  switch (codec_properity->codec_id) {
    case CodecId::AV_CODEC_ID_H264: {
      *codec_properity->H264() = VideoEncoder::GetDefaultH264Specific();
      break;
    }
    case CodecId::AV_CODEC_ID_H265: {
      break;
    }

    case CodecId::AV_CODEC_ID_VP8: {
      break;
    }

    case CodecId::AV_CODEC_ID_VP9: {
      break;
    }

    default: {
      return false;
    }
  }

  return true;
}
}  // namespace avp
