/*
 * openh264_encoder.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "openh264_encoder.h"
#include <algorithm>
#include <memory>
#include "api/video/encoded_image.h"
#include "api/video/video_frame_type.h"
#include "base/checks.h"
#include "base/errors.h"
#include "base/logging.h"
#include "common/video_codec_property.h"
#include "third_party/openh264/src/codec/api/svc/codec_api.h"
#include "third_party/openh264/src/codec/api/svc/codec_app_def.h"
#include "third_party/openh264/src/codec/api/svc/codec_def.h"
#include "third_party/openh264/src/codec/api/svc/codec_ver.h"

namespace avp {

namespace {
VideoFrameType ConvertToVideoFrameType(EVideoFrameType type) {
  switch (type) {
    case videoFrameTypeIDR:
      return VideoFrameType::kVideoFrameKey;
    case videoFrameTypeSkip:
    case videoFrameTypeI:
    case videoFrameTypeP:
    case videoFrameTypeIPMixed:
      return VideoFrameType::kVideoFrameDelta;
    case videoFrameTypeInvalid:
      break;
  }
  return VideoFrameType::kEmptyFrame;
}

}  // namespace

OpenH264Encoder::OpenH264Encoder()
    : max_payload_size_(0),
      number_of_cores_(0),
      codec_property_(VideoCodecProperty()),
      encoder_(nullptr),
      picture_(SSourcePicture()),
      configuration_(LayerConfig()),
      encoded_image_callback_(nullptr) {}

OpenH264Encoder::~OpenH264Encoder() {
  Release();
}

status_t OpenH264Encoder::InitEncoder(const VideoCodecProperty& codec_property,
                                      const Settings& encoder_settings) {
  LOG(LS_INFO) << "InitEncoder";
  if (codec_property.codec_id != CodecId::AV_CODEC_ID_H264 ||
      codec_property.width <= 0 || codec_property.height <= 0 ||
      codec_property.frame_rate == 0) {
    return BAD_VALUE;
  }

  status_t release_ret = Release();
  if (release_ret != OK) {
    return release_ret;
  }

  max_payload_size_ = encoder_settings.max_payload_size;
  number_of_cores_ = encoder_settings.number_of_cores;
  codec_property_ = codec_property;

  //  create opehh264 encoder
  ISVCEncoder* encoder;
  if (WelsCreateSVCEncoder(&encoder) != 0) {
    LOG(LS_ERROR) << "Failed to create openh264 encoder";
    DCHECK(encoder == nullptr);
    Release();
    return UNKNOWN_ERROR;
  }
  DCHECK(encoder != nullptr);

  // int trace_level = WELS_LOG_DETAIL;
  // encoder->SetOption(ENCODER_OPTION_TRACE_LEVEL, &trace_level);

  encoder_ = encoder;

  configuration_.width = codec_property.width;
  configuration_.height = codec_property.height;
  configuration_.sending = false;
  configuration_.max_frame_rate = static_cast<float>(codec_property.frame_rate);
  configuration_.max_bps = codec_property.bit_rate_range.max;
  configuration_.target_bps = codec_property.bit_rate;
  configuration_.frame_dropping_on = codec_property.H264().frame_dropping_on;
  configuration_.key_frame_interval = codec_property.H264().key_frame_interval;
  configuration_.num_temporal_layers =
      codec_property.H264().number_of_temporal_layers;

  SEncParamExt encoder_params = CreateEncoderParams();

  // initialize encoder
  if (encoder_->InitializeExt(&encoder_params) != 0) {
    LOG(LS_ERROR) << "Failed to initialize openh264 encoder";
    Release();
    return UNKNOWN_ERROR;
  }

  // TODO(youfa): Base init params on these values before submitting.
  int video_format = EVideoFormatType::videoFormatI420;
  encoder->SetOption(ENCODER_OPTION_DATAFORMAT, &video_format);

  // TODO(youfa) remove shared ptr of EncodedImage
  encoded_image_.SetEncodedData(EncodedImageBuffer::Create());
  encoded_image_.encoded_width_ = codec_property.width;
  encoded_image_.encoded_height_ = codec_property.height;
  encoded_image_.SetSize(0);

  configuration_.SetStreamState(true);

  return OK;
}

status_t OpenH264Encoder::Release() {
  return OK;
}

status_t OpenH264Encoder::RegisterEncoderCompleteCallback(
    EncodedImageCallback* callback) {
  encoded_image_callback_ = callback;
  return OK;
}

status_t OpenH264Encoder::Encode(const std::shared_ptr<VideoFrame>& frame) {
  if (encoder_ == nullptr) {
    return NO_INIT;
  }
  if (encoded_image_callback_ == nullptr) {
    return NO_INIT;
  }
  auto frame_buffer = frame->video_frame_buffer()->ToI420();

  if (!frame_buffer) {
    LOG(LS_ERROR) << "Failed to convert frame buffer to I420";
    return BAD_VALUE;
  }
  CHECK(frame_buffer->pixel_format() == VideoFrameBuffer::PixelFormat::kI420 ||
        frame_buffer->pixel_format() == VideoFrameBuffer::PixelFormat::kI420A);

  bool send_key_frame = false;
  if (configuration_.key_frame_request && configuration_.sending) {
    send_key_frame = true;
  }

  if (send_key_frame) {
    encoder_->ForceIntraFrame(true);
    configuration_.key_frame_request = false;
  }

  picture_ = {0};
  picture_.iPicWidth = configuration_.width;
  picture_.iPicHeight = configuration_.height;
  picture_.iColorFormat = videoFormatI420;
  picture_.uiTimeStamp = frame->timestamp_us() / 1000;

  picture_.iStride[0] = frame_buffer->StrideY();
  picture_.iStride[1] = frame_buffer->StrideU();
  picture_.iStride[2] = frame_buffer->StrideV();

  picture_.pData[0] = const_cast<uint8_t*>(frame_buffer->DataY());
  picture_.pData[1] = const_cast<uint8_t*>(frame_buffer->DataU());
  picture_.pData[2] = const_cast<uint8_t*>(frame_buffer->DataV());

  // EncodeFrame output.
  SFrameBSInfo info;
  memset(&info, 0, sizeof(SFrameBSInfo));

  // Encode!
  int enc_ret = encoder_->EncodeFrame(&picture_, &info);
  if (enc_ret != 0) {
    LOG(LS_ERROR) << "OpenH264 frame encoding failed, EncodeFrame returned "
                  << enc_ret << ".";
    return UNKNOWN_ERROR;
  }

  // LOG(LS_VERBOSE) << "encode, pts:" << picture_.uiTimeStamp
  //                 << ", size:" << info.iFrameSizeInBytes
  //                 << ", layer num:" << info.iLayerNum;

  encoded_image_.encoded_width_ = configuration_.width;
  encoded_image_.encoded_height_ = configuration_.height;
  encoded_image_.SetTimestamp(frame->timestamp_us());
  encoded_image_.frame_type_ = ConvertToVideoFrameType(info.eFrameType);

  PacketizeEncodedImage(&encoded_image_, &info);
  if (encoded_image_.Size() > 0) {
    encoded_image_callback_->OnEncodedImage(encoded_image_);
  }
  return OK;
}

void OpenH264Encoder::PacketizeEncodedImage(EncodedImage* image,
                                            SFrameBSInfo* info) {
  // Calculate minimum buffer size required to hold encoded data.
  size_t required_capacity = 0;
  size_t fragments_count = 0;
  for (int layer = 0; layer < info->iLayerNum; ++layer) {
    const SLayerBSInfo& layerInfo = info->sLayerInfo[layer];
    for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++fragments_count) {
      CHECK_GE(layerInfo.pNalLengthInByte[nal], 0);
      // Ensure `required_capacity` will not overflow.
      CHECK_LE(static_cast<size_t>(layerInfo.pNalLengthInByte[nal]),
               std::numeric_limits<size_t>::max() - required_capacity);
      required_capacity += layerInfo.pNalLengthInByte[nal];
    }
  }

  auto buffer = EncodedImageBuffer::Create(required_capacity);
  image->SetEncodedData(buffer);

  const uint8_t start_code[4] = {0, 0, 0, 1};
  size_t frag = 0;
  image->SetSize(0);
  for (int layer = 0; layer < info->iLayerNum; ++layer) {
    const SLayerBSInfo& layerInfo = info->sLayerInfo[layer];
    // Iterate NAL units making up this layer, noting fragments.
    size_t layer_len = 0;
    for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++frag) {
      // Because the sum of all layer lengths, `required_capacity`, fits in a
      // `size_t`, we know that any indices in-between will not overflow.
      DCHECK_GE(layerInfo.pNalLengthInByte[nal], 4);
      DCHECK_EQ(layerInfo.pBsBuf[layer_len + 0], start_code[0]);
      DCHECK_EQ(layerInfo.pBsBuf[layer_len + 1], start_code[1]);
      DCHECK_EQ(layerInfo.pBsBuf[layer_len + 2], start_code[2]);
      DCHECK_EQ(layerInfo.pBsBuf[layer_len + 3], start_code[3]);
      layer_len += layerInfo.pNalLengthInByte[nal];
    }
    // Copy the entire layer's data (including start codes).
    memcpy(buffer->Data() + image->Size(), layerInfo.pBsBuf, layer_len);
    image->SetSize(image->Size() + layer_len);
  }
  // LOG(LS_VERBOSE) << "PacketEncodedImage, pts:" <<
  // encoded_image_.timestamp_us_
  //                 << ", size:" << encoded_image_.Size();
}

void OpenH264Encoder::RequestKeyFrame() {
  if (encoder_) {
    encoder_->ForceIntraFrame(true);
  }
}

SEncParamExt OpenH264Encoder::CreateEncoderParams() const {
  SEncParamExt encoder_params;
  encoder_->GetDefaultParams(&encoder_params);
  if (codec_property_.mode == VideoCodecMode::kRealtimeVideo) {
    encoder_params.iUsageType = CAMERA_VIDEO_REAL_TIME;
  } else if (codec_property_.mode == VideoCodecMode::kScreensharing) {
    encoder_params.iUsageType = SCREEN_CONTENT_REAL_TIME;
  }

  encoder_params.iPicWidth = configuration_.width;
  encoder_params.iPicHeight = configuration_.height;
  encoder_params.iTargetBitrate = configuration_.target_bps * 1000;

  // encoder_params.iMaxBitrate = configuration_.max_bps;
  encoder_params.iMaxBitrate = UNSPECIFIED_BIT_RATE;

  encoder_params.iRCMode = RC_BITRATE_MODE;
  // encoder_params.fMaxFrameRate = configuration_.max_frame_rate;
  encoder_params.fMaxFrameRate = 20;

  encoder_params.bEnableFrameSkip = configuration_.frame_dropping_on;
  encoder_params.uiIntraPeriod = configuration_.key_frame_interval;

  encoder_params.iMultipleThreadIdc = number_of_cores_;

  encoder_params.eSpsPpsIdStrategy = SPS_LISTING;
  encoder_params.uiMaxNalSize = 0;

  LOG(LS_INFO) << "CreateEncoderParams, width:" << encoder_params.iPicWidth
               << ", height:" << encoder_params.iPicHeight
               << ", target bitrate:" << encoder_params.iTargetBitrate
               << ", max bitrate:" << encoder_params.iMaxBitrate
               << ", max frame rate:" << encoder_params.fMaxFrameRate
               << ", frame dropping:"
               << (encoder_params.bEnableFrameSkip ? "on" : "off")
               << ", key frame interval:" << encoder_params.uiIntraPeriod
               << ", temporal layer num:" << encoder_params.iTemporalLayerNum
               << ", spatial layer num:" << encoder_params.iSpatialLayerNum
               << ", usage type:" << encoder_params.iUsageType
               << ", rc mode:" << encoder_params.iRCMode
               << ", sps pps id strategy:" << encoder_params.eSpsPpsIdStrategy
               << ", max nal size:" << encoder_params.uiMaxNalSize;

  //  encoder_params.iMaxQp = 40;
  //  encoder_params.iMinQp = 10;

  encoder_params.sSpatialLayers[0].iVideoWidth = encoder_params.iPicWidth;
  encoder_params.sSpatialLayers[0].iVideoHeight = encoder_params.iPicHeight;
  encoder_params.sSpatialLayers[0].fFrameRate = encoder_params.fMaxFrameRate;
  encoder_params.sSpatialLayers[0].iSpatialBitrate =
      encoder_params.iTargetBitrate;
  encoder_params.sSpatialLayers[0].iMaxSpatialBitrate =
      encoder_params.iMaxBitrate;
  encoder_params.iTemporalLayerNum = configuration_.num_temporal_layers;
  if (encoder_params.iTemporalLayerNum > 1) {
    // iNumRefFrame specifies total number of reference buffers to allocate.
    // For N temporal layers we need at least (N - 1) buffers to store last
    // encoded frames of all reference temporal layers.
    // Note that there is no API in OpenH264 encoder to specify exact set of
    // references to be used to prediction of a given frame. Encoder can
    // theoretically use all available reference buffers.
    encoder_params.iNumRefFrame = encoder_params.iTemporalLayerNum - 1;
  }
  LOG(LS_INFO) << "OpenH264 version is " << OPENH264_MAJOR << "."
               << OPENH264_MINOR;
  encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceNum = 1;
  encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceMode =
      SM_SIZELIMITED_SLICE;
  encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceSizeConstraint =
      static_cast<unsigned int>(max_payload_size_);

  return encoder_params;
}

void OpenH264Encoder::LayerConfig::SetStreamState(bool send_stream) {
  if (send_stream && !sending) {
    // Need a key frame if we have not sent this stream before.
    key_frame_request = true;
  }
  sending = send_stream;
}

}  // namespace avp
