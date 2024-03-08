/*
 * video_encoder_config.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 * direct copy from webrtc
 */

#ifndef VIDEO_ENCODER_CONFIG_H
#define VIDEO_ENCODER_CONFIG_H

#include <optional>
#include <string>
#include <vector>

#include "common/codec_id.h"
#include "common/video_codec_property.h"

namespace ave {
// The `VideoStreamConfig` struct describes a simulcast layer, or "stream".
struct VideoStreamConfig {
  VideoStreamConfig();
  ~VideoStreamConfig();
  VideoStreamConfig(const VideoStreamConfig& other);
  std::string ToString() const;

  // Width in pixels.
  size_t width;

  // Height in pixels.
  size_t height;

  // Frame rate in fps.
  int max_framerate;

  // Bitrate, in bps, for the stream.
  uint32_t min_bitrate_kbps;
  uint32_t target_bitrate_kbps;
  uint32_t max_bitrate_kbps;

  // Scaling factor applied to the stream size.
  // `width` and `height` values are already scaled down.
  double scale_resolution_down_by;

  // Maximum Quantization Parameter to use when encoding the stream.
  uint16_t max_qp;

  // Determines the number of temporal layers that the stream should be
  // encoded with. This value should be greater than zero.
  // TODO(brandtr): This class is used both for configuring the encoder
  // (meaning that this field _must_ be set), and for signaling the app-level
  // encoder settings (meaning that the field _may_ be set). We should separate
  // this and remove this optional instead.
  std::optional<size_t> num_temporal_layers;

  // The priority of this stream, to be used when allocating resources
  // between multiple streams.
  std::optional<double> bitrate_priority;

  std::optional<std::string> scalability_mode;

  // If this stream is enabled by the user, or not.
  bool active;
};

class VideoEncoderConfig {
 public:
  // These are reference counted to permit copying VideoEncoderConfig and be
  // kept alive until all encoder_specific_settings go out of scope.
  // TODO(kthelgason): Consider removing the need for copying VideoEncoderConfig
  // and use std::optional for encoder_specific_settings instead.
  class EncoderSpecificSettings {
   public:
    // TODO(pbos): Remove FillEncoderSpecificSettings as soon as VideoCodec is
    // not in use and encoder implementations ask for codec-specific structs
    // directly.
    void FillEncoderSpecificSettings(VideoCodecProperty* codec_struct) const;

    virtual void FillVideoCodecVp8(VP8Specific* vp8_settings) const;
    virtual void FillVideoCodecVp9(VP9Specific* vp9_settings) const;
    virtual void FillVideoCodecH264(H264Specific* h264_settings) const;

   private:
    virtual ~EncoderSpecificSettings() {}
    friend class VideoEncoderConfig;
  };

  class H264EncoderSpecificSettings : public EncoderSpecificSettings {
   public:
    explicit H264EncoderSpecificSettings(const H264Specific& specifics);
    void FillVideoCodecH264(H264Specific* h264_settings) const override;

   private:
    H264Specific specifics_;
  };

  class Vp8EncoderSpecificSettings : public EncoderSpecificSettings {
   public:
    explicit Vp8EncoderSpecificSettings(const VP8Specific& specifics);
    void FillVideoCodecVp8(VP8Specific* vp8_settings) const override;

   private:
    VP8Specific specifics_;
  };

  class Vp9EncoderSpecificSettings : public EncoderSpecificSettings {
   public:
    explicit Vp9EncoderSpecificSettings(const VP9Specific& specifics);
    void FillVideoCodecVp9(VP9Specific* vp9_settings) const override;

   private:
    VP9Specific specifics_;
  };

  enum class ContentType {
    kRealtimeVideo,
    kScreen,
  };

  class VideoStreamConfigFactoryInterface {
   public:
    // An implementation should return a std::vector<VideoStreamConfig> with the
    // wanted VideoStreamConfig settings for the given video resolution.
    // The size of the vector may not be larger than
    // `encoder_config.number_of_streams`.
    virtual std::vector<VideoStreamConfig> CreateEncoderStreams(
        int width,
        int height,
        const VideoEncoderConfig& encoder_config) = 0;

   protected:
    ~VideoStreamConfigFactoryInterface() {}
  };

  VideoEncoderConfig& operator=(VideoEncoderConfig&&) = default;
  VideoEncoderConfig& operator=(const VideoEncoderConfig&) = delete;

  // Mostly used by tests.  Avoid creating copies if you can.
  VideoEncoderConfig Copy() const { return VideoEncoderConfig(*this); }

  VideoEncoderConfig();
  VideoEncoderConfig(VideoEncoderConfig&&);
  ~VideoEncoderConfig();
  std::string ToString() const;

  CodecId codec_id;

  // std::shared_ptr<VideoStreamConfigFactoryInterface> video_stream_factory;
  // std::vector<SpatialLayer> spatial_layers;
  // ContentType content_type;
  // std::shared_ptr<const EncoderSpecificSettings> encoder_specific_settings;

  // Padding will be used up to this bitrate regardless of the bitrate produced
  // by the encoder. Padding above what's actually produced by the encoder helps
  // maintaining a higher bitrate estimate. Padding will however not be sent
  // unless the estimated bandwidth indicates that the link can handle it.
  int min_bitrate_kbps;
  int max_bitrate_kbps;
  // The bitrate priority used for all VideoStreamConfigs.
  double bitrate_priority;

  // The simulcast layer's configurations set by the application for this video
  // sender. These are modified by the video_stream_factory before being passed
  // down to lower layers for the video encoding.
  // `simulcast_layers` is also used for configuring non-simulcast (when there
  // is a single VideoStreamConfig).
  std::vector<VideoStreamConfig> simulcast_layers;

  // Max number of encoded VideoStreamConfigs to produce.
  size_t number_of_streams;

  // Indicates whether quality scaling can be used or not.
  bool is_quality_scaling_allowed;

 private:
  // Access to the copy constructor is private to force use of the Copy()
  // method for those exceptional cases where we do use it.
  VideoEncoderConfig(const VideoEncoderConfig&);
};

}  // namespace ave

#endif /* !VIDEO_ENCODER_CONFIG_H */
