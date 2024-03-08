/*
 * video_sink_interface.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_SINK_INTERFACE_H
#define VIDEO_SINK_INTERFACE_H

namespace ave {

template <typename VideoFrameT>
class VideoSinkInterface {
 public:
  virtual ~VideoSinkInterface() = default;

  virtual void OnFrame(const VideoFrameT& frame) = 0;
};

}  // namespace ave

#endif /* !VIDEO_SINK_INTERFACE_H */
