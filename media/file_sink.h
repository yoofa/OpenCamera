/*
 * file_sink.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef FILE_SINK_H
#define FILE_SINK_H
#include <fstream>
#include <memory>

#include "api/video/video_frame.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_sink_interface.h"
#include "api/video/yuyv_buffer.h"
#include "base/logging.h"
#include "common/buffer.h"

namespace avp {

class FileSink : public VideoSinkInterface<std::shared_ptr<VideoFrame>> {
 public:
  explicit FileSink(const char* file) { file_.open(file); }
  ~FileSink() { file_.close(); }

  void OnFrame(const std::shared_ptr<VideoFrame>& frame);

 private:
  std::ofstream file_;
};

}  // namespace avp

#endif /* !FILE_SINK_H */
