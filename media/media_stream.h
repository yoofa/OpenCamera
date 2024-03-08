/*
 * media_stream.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef MEDIA_STREAM_H
#define MEDIA_STREAM_H

#include <string>

namespace ave {

class MediaStream {
 public:
  enum class Type { kAudio, kVideo, kData };

  MediaStream() = default;
  virtual ~MediaStream() = default;

  std::string id() const { return id_; }
  void set_id(const std::string& id) { id_ = id; }

 private:
  std::string id_;
};

}  // namespace ave

#endif /* !MEDIA_STREAM_H */
