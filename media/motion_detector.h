/*
 * motion_detector.h
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include "media/video_sink.h"

namespace avp {

class MotionDetector : public VideoSink {
 public:
  MotionDetector();
  virtual ~MotionDetector();

  void OnFps(int fps);

  /* VideoSink */
  void OnFrame(std::shared_ptr<Buffer> buffer) override;

 private:
};

}  // namespace avp

#endif /* !MOTION_DETECTOR_H */
