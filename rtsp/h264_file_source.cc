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

namespace avp {

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
    : mMeta(std::make_shared<MetaData>()), mBuf(new uint8_t[FRAME_MAX_SIZE]) {
  mMeta->setCString(kKeyMIMEType, "video/AVC");
  mMeta->setInt32(kKeyCodecType, kKeyAVCC);
  mMeta->setInt32(kKeyWidth, 960);
  mMeta->setInt32(kKeyHeight, 408);

  mFd = open(path, O_LARGEFILE | O_RDONLY);
  if (mFd <= 0) {
    LOG(LS_ERROR) << "open file error";
  }
}

H264FileSource::~H264FileSource() {
  if (mFd > 0) {
    ::close(mFd);
    mFd = -1;
  }
}

status_t H264FileSource::start(MetaData* params) {
  return OK;
}

status_t H264FileSource::stop() {
  return OK;
}

std::shared_ptr<MetaData> H264FileSource::getFormat() {
  return mMeta;
}

status_t H264FileSource::read(std::shared_ptr<Buffer>& buffer,
                              const ReadOptions* options) {
  if (mFd <= 0) {
    return UNKNOWN_ERROR;
  }

  int rSize, frameSize;
  uint8_t* nextStartCodePos;
  rSize = ::read(mFd, mBuf.get(), FRAME_MAX_SIZE);
  if (!startCode3(mBuf.get()) && !startCode4(mBuf.get())) {
    return -1;
  }

  nextStartCodePos = findNextStartCode(mBuf.get() + 3, rSize - 3);

  if (!nextStartCodePos) {
    lseek(mFd, 0, SEEK_SET);
    frameSize = rSize;
  } else {
    frameSize = nextStartCodePos - mBuf.get();
    lseek(mFd, frameSize - rSize, SEEK_CUR);
  }
  std::shared_ptr<Buffer> buf = std::make_shared<Buffer>(frameSize);
  memcpy(buf->data(), mBuf.get(), frameSize);
  buf->meta()->setInt64("timeUs", Looper::getNowUs());
  buffer = buf;

  return OK;
}

}  // namespace avp
