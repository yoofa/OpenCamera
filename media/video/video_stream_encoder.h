/*
 * video_stream_encoder.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_STREAM_ENCODER_H
#define VIDEO_STREAM_ENCODER_H

#include "api/video/encoded_image.h"
#include "api/video/video_stream_encoder_interface.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "base/constructor_magic.h"
#include "base/sequence_checker.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/thread_annotation.h"

namespace ave {
class VideoStreamEncoder : public VideoStreamEncoderInterface {
 public:
  VideoStreamEncoder(base::TaskRunnerFactory* task_runner_factory,
                     VideoEncoderFactory* encoder_factory,
                     EncodedImageCallback* sink);
  ~VideoStreamEncoder() override;

  // VideoStreamEncoderInterface implementation.
  void SetSource(
      VideoSourceInterface<std::shared_ptr<VideoFrame>>* source) override;

  void SetSink(EncoderSink* sink) override;

  void SetStartBitrate(int start_bitrate_bps) override;

  void SendKeyFrame() override;

  void ConfigureEncoder(VideoEncoderConfig config,
                        size_t max_data_payload_length) override;

  // VideoSinkInterface implementation.
  void OnFrame(const std::shared_ptr<VideoFrame>& frame) override;

  void Stop() override;

 private:
  class VideoFrameInfo {
   public:
    VideoFrameInfo(size_t width, size_t height, VideoFrameBuffer::Type type)
        : width(width), height(height), type(type) {}
    size_t width;
    size_t height;
    VideoFrameBuffer::Type type;
    size_t pixel_count() const { return width * height; }
  };

  void MaybeEncodeVideoFrame(const std::shared_ptr<VideoFrame>& frame);
  void EncodeVideoFrame(const std::shared_ptr<VideoFrame>& frame);

  // Create or reconfigure the encoder.
  void ReConfigureEncoder() AVE_RUN_ON(&encoder_runner_);

  void ReleaseEncoder() AVE_RUN_ON(&encoder_runner_);

  base::TaskRunnerFactory* task_runner_factory_;
  VideoEncoderFactory* encoder_factory_;
  EncodedImageCallback* sink_ GUARDED_BY(&encoder_runner_);

  VideoEncoderConfig encoder_config_ GUARDED_BY(&encoder_runner_);
  std::unique_ptr<VideoEncoder> encoder_ GUARDED_BY(&encoder_runner_);
  bool encoder_initialized_ GUARDED_BY(&encoder_runner_);

  // Set when ConfigureEncoder has been called in order to lazy reconfigure
  // the encoder on the next frame.
  bool pending_encoder_reconfiguration_ GUARDED_BY(&encoder_runner_);
  // Set when configuration must create a new encoder object, e.g.,
  // because of a codec change.
  bool pending_encoder_creation_ GUARDED_BY(&encoder_runner_);

  std::optional<VideoFrameInfo> last_frame_info_ GUARDED_BY(&encoder_runner_);

  size_t max_data_payload_length_ GUARDED_BY(&encoder_runner_);

  base::TaskRunner encoder_runner_;

  AVE_DISALLOW_COPY_AND_ASSIGN(VideoStreamEncoder);
};

}  // namespace ave

#endif /* !VIDEO_STREAM_ENCODER_H */
