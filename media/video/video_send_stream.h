/*
 * video_send_stream.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_SEND_STREAM_H
#define VIDEO_SEND_STREAM_H

#include "api/video/video_stream_encoder_interface.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/thread_annotation.h"
#include "media/media_stream.h"
#include "media/video/video_stream_sender.h"

namespace avp {

class VideoSendStream : public MediaStream,
                        public VideoStreamEncoderInterface::EncoderSink {
  using VideoSource =
      std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>;

 public:
  VideoSendStream(
      base::TaskRunnerFactory* task_runner_factory,
      base::TaskRunner* task_runner,
      VideoEncoderFactory* video_encoder_factory,
      VideoSourceInterface<std::shared_ptr<VideoFrame>>* video_source,
      VideoStreamSender* video_stream_sender,
      VideoEncoderConfig encoder_config);

  ~VideoSendStream();

  void Start();
  void Stop();

  void RequestKeyFrame();

  // VideoStreamEncoderInterface::EncoderSink implementation.
  Result OnEncodedImage(const EncodedImage& encoded_image) override;

 private:
  void ReConfigureEncoder(VideoEncoderConfig config);

  base::TaskRunnerFactory* task_runner_factory_;
  base::TaskRunner* task_runner_;
  VideoEncoderFactory* video_encoder_factory_;
  VideoStreamSender* video_stream_sender_;
  VideoSourceInterface<std::shared_ptr<VideoFrame>>* video_source_;

  std::unique_ptr<VideoStreamEncoderInterface> video_stream_encoder_;
};

}  // namespace avp

#endif /* !VIDEO_SEND_STREAM_H */
