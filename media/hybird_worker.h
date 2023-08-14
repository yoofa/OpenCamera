/*
 * hybird_worker.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef HYBIRD_WORKER_H
#define HYBIRD_WORKER_H

#include <memory>
#include <vector>

#include "api/video_codecs/video_encoder_factory.h"
#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/thread_annotation.h"
#include "media/media_transport.h"
#include "media/media_worker.h"
#include "media/video_send_stream.h"
#include "media/video_stream_sender.h"

namespace avp {

class HybirdWorker : public MediaWorker {
  using VideoSource =
      std::shared_ptr<VideoSourceInterface<std::shared_ptr<VideoFrame>>>;
  using EncodedVideoSink =
      std::shared_ptr<VideoSinkInterface<std::shared_ptr<EncodedImage>>>;

 public:
  explicit HybirdWorker(VideoEncoderFactory* video_encoder_factory);
  virtual ~HybirdWorker();

  void AddVideoSource(VideoSource& video_source, int32_t stream_id) override;
  void RemoveVideoSource(VideoSource& video_source, int32_t stream_id) override;

  void AddEncodedVideoSink(EncodedVideoSink& encoded_image_sink,
                           int32_t stream_id) override;
  void RemoveEncodedVideoSink(EncodedVideoSink& encoded_image_sink,
                              int32_t stream_id) override;

  struct VideoSourceInfo {
    VideoSource video_source;
    int32_t stream_id;
  };
  struct VideoSendStreamInfo {
    std::unique_ptr<VideoSendStream> video_send_stream;
    int32_t stream_id;
  };

 private:
  // capture runner, used to handle capture task
  std::unique_ptr<base::TaskRunnerFactory> task_runner_factory_;
  base::TaskRunner worker_task_runner_;

  std::unique_ptr<MediaTransport> media_transport_
      GUARDED_BY(worker_task_runner_);

  std::vector<VideoSourceInfo> video_sources_ GUARDED_BY(worker_task_runner_);
  std::vector<VideoSendStreamInfo> video_send_streams_
      GUARDED_BY(worker_task_runner_);
};

}  // namespace avp

#endif /* !HYBIRD_WORKER_H */
