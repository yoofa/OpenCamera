/*
 * video_stream_sender.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_STREAM_SENDER_H
#define VIDEO_STREAM_SENDER_H

#include "api/video/encoded_image.h"
#include "api/video/video_sink_interface.h"
#include "base/mutex.h"
#include "base/task_util/task.h"
#include "base/task_util/task_runner.h"
#include "base/thread_annotation.h"
#include "media/video/video_broadcaster.h"
#include "media/video/video_stream_sender_interface.h"

namespace avp {

class VideoStreamSender : public VideoStreamSenderInterface {
 public:
  VideoStreamSender(base::TaskRunner* transport_runner);
  ~VideoStreamSender();

  bool frame_wanted() const;

  void AddVideoSink(
      const std::shared_ptr<VideoSinkInterface<EncodedImage>>& sink);
  void RemoveVideoSink(
      const std::shared_ptr<VideoSinkInterface<EncodedImage>>& sink);

  // VideoStreamSenderInterface implementation.
  Result OnEncodedImage(const EncodedImage& encoded_image) override;

 private:
  base::TaskRunner* transport_runner_;
  mutable Mutex sink_lock_;
  std::vector<std::shared_ptr<VideoSinkInterface<EncodedImage>>> sinks_
      GUARDED_BY(sink_lock_);

  VideoBroadcaster<EncodedImage> broadcaster_;
};

}  // namespace avp

#endif /* !VIDEO_STREAM_SENDER_H */
