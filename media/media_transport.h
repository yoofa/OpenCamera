/*
 * media_transport.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_TRANSPORT_H
#define MEDIA_TRANSPORT_H

#include <memory>
#include <ostream>

#include "base/task_util/task_runner.h"
#include "base/task_util/task_runner_factory.h"
#include "base/thread_annotation.h"
#include "media/video_stream_sender.h"

namespace avp {
class MediaTransport {
  using EncodedVideoSink =
      std::shared_ptr<VideoSinkInterface<std::shared_ptr<EncodedImage>>>;

 public:
  MediaTransport(base::TaskRunnerFactory* task_runner_factory);
  ~MediaTransport();

  VideoStreamSender* GetVideoStreamSender(int32_t id)
      //    REQUIRES(transport_runner_)
      ;
  void RemoveVideoStreamSender(int32_t id);

  void AddVideoSink(const EncodedVideoSink& sink, int32_t id);
  void RemoveVideoSink(const EncodedVideoSink& sink);

  bool frame_wanted(int32_t stream_id) const;

 private:
  struct VideoStreamSenderInfo {
    std::unique_ptr<VideoStreamSender> video_stream_sender;
    int32_t id;
  };

  base::TaskRunner transport_runner_;
  std::vector<VideoStreamSenderInfo> video_stream_senders_;
};

}  // namespace avp

#endif /* !MEDIA_TRANSPORT_H */
