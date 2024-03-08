/*
 * fdkaac_encoder.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "fdkaac_encoder.h"

#include "base/errors.h"
#include "base/logging.h"
#include "base/types.h"
#include "common/media_errors.h"
#include "common/media_packet.h"
#include "third_party/fdk-aac/src/libAACenc/include/aacenc_lib.h"
#include "third_party/fdk-aac/src/libAACenc/src/aacenc.h"
#include "third_party/fdk-aac/src/libSYS/include/FDK_audio.h"

namespace ave {

namespace {
static CHANNEL_MODE getChannelMode(uint32_t nChannels) {
  CHANNEL_MODE chMode = MODE_INVALID;
  switch (nChannels) {
    case 1:
      chMode = MODE_1;
      break;
    case 2:
      chMode = MODE_2;
      break;
    case 3:
      chMode = MODE_1_2;
      break;
    case 4:
      chMode = MODE_1_2_1;
      break;
    case 5:
      chMode = MODE_1_2_2;
      break;
    case 6:
      chMode = MODE_1_2_2_1;
      break;
    default:
      chMode = MODE_INVALID;
  }
  return chMode;
}

}  // namespace

FDKAACEncoder::FDKAACEncoder() : callback_(nullptr), encoder_(nullptr) {
  aacEncOpen(&encoder_, 0, 0);
}

FDKAACEncoder::~FDKAACEncoder() {
  aacEncClose(&encoder_);
}

status_t FDKAACEncoder::InitEncoder(const AudioCodecProperty& codec_settings) {
  AVE_LOG(LS_INFO) << "FDKAACEncoder::InitEncoder";
  if (codec_settings.codec_id != CodecId::AV_CODEC_ID_AAC) {
    return INVALID_OPERATION;
  }

  if (AACENC_OK != aacEncoder_SetParam(encoder_, AACENC_AOT, 2)) {
    AVE_LOG(LS_ERROR) << "Failed to set AAC encoder profile";
    return UNKNOWN_ERROR;
  }

  if (aacEncoder_SetParam(encoder_, AACENC_SAMPLERATE,
                          codec_settings.sample_rate) != AACENC_OK) {
    AVE_LOG(LS_ERROR) << "Failed to set AAC encoder sample_rate";
    return UNKNOWN_ERROR;
  }

  if (aacEncoder_SetParam(encoder_, AACENC_BITRATE, codec_settings.bit_rate) !=
      AACENC_OK) {
    AVE_LOG(LS_ERROR) << "Failed to set AAC encoder bitrate";
    return UNKNOWN_ERROR;
  }

  if (aacEncoder_SetParam(encoder_, AACENC_CHANNELMODE,
                          getChannelMode(codec_settings.channels)) !=
      AACENC_OK) {
    AVE_LOG(LS_ERROR) << "Failed to set AAC encoder channel mode, channels:"
                      << codec_settings.channels;
    return UNKNOWN_ERROR;
  }

  if (aacEncoder_SetParam(encoder_, AACENC_CHANNELORDER, 1) != AACENC_OK) {
    AVE_LOG(LS_ERROR) << "Failed to set AAC encoder channel order";
    return UNKNOWN_ERROR;
  }

  if (AACENC_OK != aacEncoder_SetParam(encoder_, AACENC_TRANSMUX, TT_MP4_RAW)) {
    AVE_LOG(LS_ERROR) << "Failed to set AAC encoder parameters";
    return UNKNOWN_ERROR;
  }

  // if (mSBRMode != -1 && mAACProfile == OMX_AUDIO_AACObjectELD) {
  //   if (AACENC_OK != aacEncoder_SetParam(encoder_, AACENC_SBR_MODE,
  //   mSBRMode)) {
  //     ALOGE("Failed to set AAC encoder parameters");
  //     return UNKNOWN_ERROR;
  //   }
  // }

  /* SBR ratio parameter configurations:
     0: Default configuration wherein SBR ratio is configured depending on
     audio object type by the FDK. 1: Downsampled SBR (default for ELD) 2:
     Dualrate SBR (default for HE-AAC)
   */
  // if (AACENC_OK != aacEncoder_SetParam(encoder_, AACENC_SBR_RATIO,
  // mSBRRatio)) {
  //   ALOGE("Failed to set AAC encoder parameters");
  //   return UNKNOWN_ERROR;
  // }
  //
  if (aacEncEncode(encoder_, NULL, NULL, NULL, NULL) != AACENC_OK) {
    AVE_LOG(LS_ERROR) << "Unable to initialize the encoder";
    return UNKNOWN_ERROR;
  }

  AACENC_InfoStruct info = {0};
  if (aacEncInfo(encoder_, &info) != AACENC_OK) {
    AVE_LOG(LS_ERROR) << "Unable to get the encoder info";
    return UNKNOWN_ERROR;
  }

  AVE_LOG(LS_INFO) << "AAC Encoder Info, max_output_size:"
                   << info.maxOutBufBytes
                   << ", frame_length:" << info.frameLength
                   << ", input_channels:" << info.inputChannels;

  return OK;
}

status_t FDKAACEncoder::RegisterEncoderCompleteCallback(
    EncodedCallback* callback) {
  callback_ = callback;
  return OK;
}

status_t FDKAACEncoder::Release() {
  return OK;
}

status_t FDKAACEncoder::Encode(const std::shared_ptr<AudioFrame>& frame) {
  AVE_LOG(LS_VERBOSE) << "FDKAACEncoder::Encode";
  AACENC_BufDesc in_buf = {0}, out_buf = {0};
  AACENC_InArgs in_args = {0};
  AACENC_OutArgs out_args = {0};

  std::shared_ptr<AudioFrame> out_frame = std::make_shared<AudioFrame>();

  size_t in_size = frame->num_channels() * frame->samples_per_channel() * 2;
  in_args.numInSamples = in_size;

  uint8_t* in_ptr = (unsigned char*)frame->mutable_data();
  void* in_buffer[] = {in_ptr};
  INT in_buffer_ids[] = {IN_AUDIO_DATA};
  INT in_buffer_size[] = {(INT)in_size};
  INT in_buffer_el_size[] = {sizeof(int16_t)};

  in_buf.numBufs = sizeof(in_buffer) / sizeof(void*);
  in_buf.bufs = (void**)&in_buffer;
  in_buf.bufferIdentifiers = in_buffer_ids;
  in_buf.bufSizes = in_buffer_size;
  in_buf.bufElSizes = in_buffer_el_size;

  uint8_t* out_ptr = (unsigned char*)(out_frame->mutable_data());

  size_t out_size_available = out_frame->max_16bit_samples();
  void* out_buffer[] = {out_ptr};
  INT out_buffer_ids[] = {OUT_BITSTREAM_DATA};
  INT out_buffer_size[] = {0};
  INT out_buffer_el_size[] = {sizeof(UCHAR)};

  out_buf.numBufs = sizeof(out_buffer) / sizeof(void*);
  out_buf.bufs = (void**)&out_buffer;
  out_buf.bufferIdentifiers = out_buffer_ids;
  out_buf.bufSizes = out_buffer_size;
  out_buf.bufElSizes = out_buffer_el_size;

  AACENC_ERROR enc_err = AACENC_OK;
  size_t out_bytes = 0;

  out_buffer_size[0] = out_size_available - out_bytes;

  do {
    memset(&out_args, 0, sizeof(out_args));
    out_buffer[0] = out_ptr;
    out_buffer_size[0] = out_size_available - out_bytes;

    enc_err = aacEncEncode(encoder_, &in_buf, &out_buf, &in_args, &out_args);

    if (enc_err == AACENC_OK) {
      out_ptr += out_args.numOutBytes;
      out_bytes += out_args.numOutBytes;

      if (out_args.numInSamples > 0) {
        size_t num_remaining = in_args.numInSamples - out_args.numInSamples;
        if (num_remaining > 0) {
          in_ptr += out_args.numInSamples;
          in_args.numInSamples = num_remaining;
        }
      }
    }

  } while (enc_err == AACENC_OK && out_args.numOutBytes > 0);

  if (enc_err != AACENC_OK || out_bytes == 0) {
    AVE_LOG(LS_ERROR) << "Failed to encode frame, error code:" << enc_err;
    return UNKNOWN_ERROR;
  }

  MediaPacket packet = MediaPacket::Create(out_bytes);
  packet.SetMediaType(MediaType::AUDIO);
  packet.SetData((uint8_t*)out_frame->data(), out_bytes);
  auto packet_info = packet.audio_info();
  AVE_DCHECK(packet_info != nullptr);
  packet_info->timestamp_us = frame->timestamp_;
  packet_info->codec_id = CodecId::AV_CODEC_ID_AAC;
  packet_info->sample_rate_hz = frame->sample_rate_hz_;
  packet_info->channels = frame->num_channels_;
  packet_info->samples_per_channel = frame->samples_per_channel_;
  packet_info->bits_per_sample = 16;

  AVE_LOG(LS_VERBOSE) << "packet.size:" << out_bytes
                      << ", ts:" << packet_info->timestamp_us;

  if (callback_) {
    callback_->OnEncoded(packet);
  }

  // std::shared_ptr<Buffer8> buffer = std::make_shared<Buffer8>(out_bytes);
  // memcpy(buffer->data(), out_frame->data(), out_bytes);

  // if (callback_) {
  //   callback_->OnEncoded(buffer);
  // }

  return OK;
}

}  // namespace ave
