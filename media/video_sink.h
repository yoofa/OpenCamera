/*
 * video_sink.h
 * Copyright (C) 2023 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef VIDEO_SINK_H
#define VIDEO_SINK_H

#include <memory>

#include "common/message.h"

namespace avp {

class VideoSink : public MessageObject {
 public:
  virtual ~VideoSink() = default;

  virtual void OnFrame(std::shared_ptr<Buffer> buffer) = 0;

  void SetNotify(std::shared_ptr<Message> notify) {
    notify_ = std::move(notify);
  }

  std::shared_ptr<Message> DupNotify() {
    if (notify_ != nullptr) {
      return notify_->dup();
    }
    return nullptr;
  }

 private:
  std::shared_ptr<Message> notify_;
};

}  // namespace avp

#endif /* !VIDEO_SINK_H */
