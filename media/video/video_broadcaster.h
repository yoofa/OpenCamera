/*
 * video_broadcaster.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_BROADCASTER_H
#define VIDEO_BROADCASTER_H

#include "api/video/video_sink_interface.h"
#include "base/mutex.h"
#include "base/thread_annotation.h"
#include "media/video/video_source_base.h"

namespace avp {

template <typename VideoFrameT>
class VideoBroadcaster : public VideoSourceBase<VideoFrameT>,
                         public VideoSinkInterface<VideoFrameT> {
 public:
  // Returns true if the next frame will be delivered to at least one sink.
  bool frame_wanted() const {
    lock_guard l(lock_);
    return !this->sink_pairs().empty();
  }

  // VideoSourceInterface implementation.
  void AddOrUpdateSink(VideoSinkInterface<VideoFrameT>* sink,
                       const VideoSinkWants& wants) override {
    DCHECK(sink != nullptr);
    lock_guard l(lock_);
    VideoSourceBase<VideoFrameT>::AddOrUpdateSink(sink, wants);
  }

  void RemoveSink(VideoSinkInterface<VideoFrameT>* sink) override {
    DCHECK(sink != nullptr);
    lock_guard l(lock_);
    VideoSourceBase<VideoFrameT>::RemoveSink(sink);
  }

  // VideoSinkInterface implementation.
  void OnFrame(const VideoFrameT& frame) override {
    lock_guard l(lock_);
    for (auto& sink_pair : this->sink_pairs()) {
      sink_pair.sink->OnFrame(frame);
    }
  }

 private:
  mutable Mutex lock_;
};

}  // namespace avp

#endif /* !VIDEO_BROADCASTER_H */
