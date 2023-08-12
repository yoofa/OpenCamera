/*
 * video_capturer_unittest.cc
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#include <chrono>
#include <iostream>
#include <thread>

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "base/logging.h"
#include "media/video_capturer.h"
#include "test/gtest.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

namespace avp {
using namespace std::chrono_literals;
namespace {

constexpr size_t kWidth = 640;
constexpr size_t kHeight = 480;

struct TestParameter {
  int i = 0;
  int j = 0;
};

class TestVideoSource
    : public VideoSourceInterface<std::shared_ptr<VideoFrame>> {
 public:
  TestVideoSource() : sink_(nullptr), frame_sent_(0) {}
  ~TestVideoSource() override = default;

  void AddOrUpdateSink(VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink,
                       const VideoSinkWants& wants) override {
    sink_ = sink;
  }

  void RemoveSink(
      VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink) override {
    sink_ = nullptr;
  }

  void ProduceOneFrame(size_t width, size_t height) {
    auto frame = std::make_shared<VideoFrame>(
        frame_sent_++, I420Buffer::Create(width, height), 0, std::nullopt);

    if (sink_) {
      sink_->OnFrame(frame);
    }
  }

  VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink_;

  int frame_sent_;
};

class TestVideoSink : public VideoSinkInterface<std::shared_ptr<VideoFrame>> {
 public:
  TestVideoSink() : frame_received_(0) {}
  ~TestVideoSink() override = default;

  void OnFrame(const std::shared_ptr<VideoFrame>& frame) override {
    EXPECT_EQ(kWidth, frame->width());
    frame_received_++;
  }
  uint64_t frame_received() const { return frame_received_; }

  uint64_t frame_received_;
};

class TestVideoProcessor
    : public VideoProcessorInterface<std::shared_ptr<VideoFrame>> {
 public:
  TestVideoProcessor() : frame_received_(0) {}

  ~TestVideoProcessor() = default;

  void AddOrUpdateSink(VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink,
                       const VideoSinkWants& wants) override {
    sink_ = sink;
  }

  void RemoveSink(
      VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink) override {
    sink_ = nullptr;
  }

  void OnFrame(const std::shared_ptr<VideoFrame>& frame) override {
    frame_received_++;
    EXPECT_EQ(kWidth, frame->width());
    if (sink_) {
      sink_->OnFrame(frame);
    }
  }

  void OnProcess(const std::shared_ptr<VideoFrame>& frame) override {}

  uint64_t frame_received() const { return frame_received_; }

 private:
  VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink_;
  uint64_t frame_received_;
};

}  // namespace

TEST(VideoCapturerTest, VideoSourceTest) {
  VideoCapturer capturer(nullptr);
  TestVideoSource source;
  EXPECT_EQ((uint64_t)0, capturer.frame_received());
  capturer.SetVideoSource(&source, VideoSinkWants());
  std::this_thread::sleep_for(100ms);
  source.ProduceOneFrame(kWidth, kHeight);
  std::this_thread::sleep_for(100ms);
  EXPECT_EQ((uint64_t)1, capturer.frame_received());
}

TEST(VideoCapturerTest, VideoSinkTest) {
  VideoCapturer capturer(nullptr);
  TestVideoSource source;
  TestVideoSink sink1;
  TestVideoSink sink2;
  EXPECT_EQ((uint64_t)0, capturer.frame_received());
  EXPECT_EQ((uint64_t)0, sink1.frame_received());
  EXPECT_EQ((uint64_t)0, sink2.frame_received());

  capturer.SetVideoSource(&source, VideoSinkWants());
  std::this_thread::sleep_for(100ms);

  source.ProduceOneFrame(kWidth, kHeight);
  std::this_thread::sleep_for(100ms);
  EXPECT_EQ((uint64_t)1, capturer.frame_received());
  EXPECT_EQ((uint64_t)0, capturer.frame_sent());
  EXPECT_EQ((uint64_t)0, sink1.frame_received());
  EXPECT_EQ((uint64_t)0, sink2.frame_received());

  capturer.AddOrUpdateSink(&sink1, VideoSinkWants());
  capturer.AddOrUpdateSink(&sink2, VideoSinkWants());
  source.ProduceOneFrame(kWidth, kHeight);
  std::this_thread::sleep_for(100ms);
  EXPECT_EQ((uint64_t)2, capturer.frame_received());
  EXPECT_EQ((uint64_t)1, capturer.frame_sent());
  EXPECT_EQ((uint64_t)1, sink1.frame_received());
  EXPECT_EQ((uint64_t)1, sink2.frame_received());

  capturer.RemoveSink(&sink1);
  source.ProduceOneFrame(kWidth, kHeight);
  std::this_thread::sleep_for(100ms);
  EXPECT_EQ((uint64_t)3, capturer.frame_received());
  EXPECT_EQ((uint64_t)2, capturer.frame_sent());
  EXPECT_EQ((uint64_t)1, sink1.frame_received());
  EXPECT_EQ((uint64_t)2, sink2.frame_received());

  capturer.RemoveSink(&sink2);
  source.ProduceOneFrame(kWidth, kHeight);
  std::this_thread::sleep_for(100ms);
  EXPECT_EQ((uint64_t)4, capturer.frame_received());
  EXPECT_EQ((uint64_t)2, capturer.frame_sent());
  EXPECT_EQ((uint64_t)1, sink1.frame_received());
  EXPECT_EQ((uint64_t)2, sink2.frame_received());
}

TEST(VideoCapturerTest, VideoProcessorTest) {
  TestVideoSink sink;
  TestVideoProcessor processor;
}

}  // namespace avp
