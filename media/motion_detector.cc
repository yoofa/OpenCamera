/*
 * motion_detector.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "motion_detector.h"

namespace avp {

MotionDetector::MotionDetector() {}

MotionDetector::~MotionDetector() {}

void MotionDetector::OnFps(int32_t fps) {
  auto notify = DupNotify();
  notify->setInt32("fps", fps);
  notify->post();
}

void MotionDetector::OnFrame(std::shared_ptr<Buffer> buffer) {}

}  // namespace avp
