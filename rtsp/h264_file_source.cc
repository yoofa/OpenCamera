/*
 * h264_file_source.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include "h264_file_source.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "base/logging.h"
#include "common/buffer.h"
#include "common/meta_data.h"
#include "common/utils.h"

namespace ave {

static inline bool startCode3(uint8_t* buf) {
  if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
    return true;
  else
    return false;
}

static inline bool startCode4(uint8_t* buf) {
  if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
    return true;
  else
    return false;
}

static uint8_t* findNextStartCode(uint8_t* buf, int len) {
  int i;
  if (len < 3)
    return nullptr;

  for (i = 0; i < len - 3; ++i) {
    if (startCode3(buf) || startCode4(buf))
      return buf;
    ++buf;
  }

  if (startCode3(buf))
    return buf;

  return nullptr;
}

H264FileSource::H264FileSource(const char* path)
    : meta_(std::make_shared<MetaData>()), buf_(new uint8_t[FRAME_MAX_SIZE]) {
  meta_->setCString(kKeyMIMEType, "video/AVC");
  meta_->setInt32(kKeyCodecType, kKeyAVCC);
  meta_->setInt32(kKeyWidth, 960);
  meta_->setInt32(kKeyHeight, 408);

  fd_ = open(path, O_LARGEFILE | O_RDONLY);
  if (fd_ <= 0) {
    AVE_LOG(LS_ERROR) << "open file error";
  }
}

H264FileSource::~H264FileSource() {
  if (fd_ > 0) {
    ::close(fd_);
    fd_ = -1;
  }
}

status_t H264FileSource::start(MetaData* params) {
  return OK;
}

status_t H264FileSource::stop() {
  return OK;
}

std::shared_ptr<MetaData> H264FileSource::getFormat() {
  return meta_;
}

status_t H264FileSource::read(std::shared_ptr<Buffer>& buffer,
                              const ReadOptions* options) {
  if (fd_ <= 0) {
    return UNKNOWN_ERROR;
  }

  int rSize, frameSize;
  uint8_t* nextStartCodePos;
  rSize = ::read(fd_, buf_.get(), FRAME_MAX_SIZE);
  if (!startCode3(buf_.get()) && !startCode4(buf_.get())) {
    return -1;
  }

  nextStartCodePos = findNextStartCode(buf_.get() + 3, rSize - 3);

  if (!nextStartCodePos) {
    lseek(fd_, 0, SEEK_SET);
    frameSize = rSize;
  } else {
    frameSize = nextStartCodePos - buf_.get();
    lseek(fd_, frameSize - rSize, SEEK_CUR);
  }
  std::shared_ptr<Buffer> buf = std::make_shared<Buffer>(frameSize);
  memcpy(buf->data(), buf_.get(), frameSize);
  buf->meta()->setInt64("timeUs", Looper::getNowUs());
  buffer = buf;

  return OK;
}

}  // namespace ave
