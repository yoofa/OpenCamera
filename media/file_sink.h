/*
 * file_sink.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef FILE_SINK_H
#define FILE_SINK_H
#include <fstream>

#include "base/logging.h"
#include "common/buffer.h"
#include "media/video_sink.h"

namespace avp {

class FileSink : public VideoSink {
 public:
  explicit FileSink(const char* file) { file_.open(file); }
  ~FileSink() { file_.close(); }

  void OnFrame(std::shared_ptr<Buffer> buffer) override {
    if (buffer != nullptr) {
      file_.write((char*)buffer->data(), buffer->size());
    }
  }

 private:
  std::ofstream file_;
};

}  // namespace avp

#endif /* !FILE_SINK_H */
