/*
 * video_source_base.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_SOURCE_BASE_H
#define VIDEO_SOURCE_BASE_H

#include "api/video/video_sink_interface.h"
#include "api/video/video_source_interface.h"
#include "base/logging.h"

namespace avp {

template <typename VideoFrameT>
class VideoSourceBase : public VideoSourceInterface<VideoFrameT> {
 public:
  VideoSourceBase() = default;
  ~VideoSourceBase() override = default;

  virtual void AddOrUpdateSink(VideoSinkInterface<VideoFrameT>* sink,
                               const VideoSinkWants& wants) override {
    DCHECK(sink != nullptr);
    SinkPair* sink_pair = FindSinkPair(sink);
    if (sink_pair) {
      sink_pair->wants = wants;
      return;
    }
    sinks_.push_back(SinkPair(sink, wants));
  }

  virtual void RemoveSink(VideoSinkInterface<VideoFrameT>* sink) override {
    DCHECK(FindSinkPair(sink));
    sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(),
                                [sink](const SinkPair& sink_pair) {
                                  return sink_pair.sink == sink;
                                }),
                 sinks_.end());
  }

  struct SinkPair {
    SinkPair(VideoSinkInterface<VideoFrameT>* sink, VideoSinkWants wants)
        : sink(sink), wants(wants) {}
    VideoSinkInterface<VideoFrameT>* sink;
    VideoSinkWants wants;
  };

  SinkPair* FindSinkPair(const VideoSinkInterface<VideoFrameT>* sink) {
    DCHECK(sink != nullptr);

    auto sink_pair_it = std::find_if(
        sinks_.begin(), sinks_.end(),
        [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; });

    if (sink_pair_it != sinks_.end()) {
      return &(*sink_pair_it);
    }
    return nullptr;
  }

  const std::vector<SinkPair>& sink_pairs() const { return sinks_; }

 private:
  std::vector<SinkPair> sinks_;
};

}  // namespace avp

#endif /* !VIDEO_SOURCE_BASE_H */
