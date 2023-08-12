/*
 * camera_info.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include <unistd.h>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>

#include "base/logging.h"
#include "common/buffer.h"
#include "common/codec_constants.h"
#include "common/handler.h"
#include "common/message.h"
#include "common/meta_data.h"
#include "media/camera_info/default_video_source_factory.h"
#include "v4l2_video_source.h"
#include "video_source_factory.h"

using namespace avp;

std::mutex mutex_;
std::condition_variable condition_;

class ImageWriter : public Handler {
 public:
  ImageWriter(const char* file)
      : mLooper(std::make_shared<Looper>()), mImage(nullptr) {
    mLooper->setName("ImageWriter");
    mImage.open(file);
  }
  virtual ~ImageWriter() {
    if (mImage) {
      mImage.close();
    }
  }

  status_t init() {
    mLooper->start();
    mLooper->registerHandler(shared_from_this());
    return OK;
  }

  void onMessageReceived(const std::shared_ptr<Message>& message) override;

  enum {
    kWhatVideoFrame = 'vdfr',
    kWhatStop = 'stop',

  };

 private:
  std::shared_ptr<Looper> mLooper;
  std::ofstream mImage;
};

void ImageWriter::onMessageReceived(const std::shared_ptr<Message>& message) {
  switch (message->what()) {
    case kWhatVideoFrame: {
      std::shared_ptr<Buffer> buffer;
      message->findBuffer("buffer", buffer);
      if (buffer.get() != nullptr) {
        mImage.write((char*)buffer->data(), buffer->size());
      }

      break;
    }

    case kWhatStop: {
      condition_.notify_all();
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  avp::LogMessage::LogToDebug(avp::LogSeverity::LS_VERBOSE);

  auto writer = std::make_shared<ImageWriter>("frame.yuv");
  writer->init();

  // auto source(std::make_unique<V4L2VideoSource>(argv[1]));
  auto source_factory = CreateDefaultVideoSourceFactory();
  auto video_info = std::make_shared<Message>();
  video_info->setString("v4l2-dev", argv[1]);
  auto source = source_factory->CreateVideoSource(video_info);

  MetaData metaData;
  metaData.setInt32(kKeyWidth, 1920);
  metaData.setInt32(kKeyHeight, 1080);
  metaData.setInt32(kKeyColorFormat, COLOR_FormatYUV420Planar);

  int ret = source->start(&metaData);

  LOG(LS_INFO) << "start ret: " << ret;

  if (ret < 0) {
    return ret;
  }

  for (int i = 0; i < 100; i++) {
    std::shared_ptr<Buffer> buffer;
    ret = source->read(buffer);
    LOG(LS_INFO) << "read ret:" << ret << ", size:" << buffer->size();
    if (ret != OK) {
      continue;
    }
    auto msg = std::make_shared<Message>(ImageWriter::kWhatVideoFrame, writer);
    msg->setBuffer("buffer", buffer);
    msg->post();

    usleep(5000);
  }
  source->stop();

  LOG(LS_INFO) << "source stop";

  auto msg = std::make_shared<Message>(ImageWriter::kWhatStop, writer);
  msg->post();

  std::unique_lock<std::mutex> l_(mutex_);
  condition_.wait(l_);
  LOG(LS_INFO) << "end";
}
