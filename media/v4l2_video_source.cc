/*
 * v4l2_video_source.cc
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */
#include "v4l2_video_source.h"

#include <fcntl.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <memory>
#include <optional>

#include "api/video/i420_buffer.h"
#include "api/video/nv12_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/yuyv_buffer.h"
#include "base/checks.h"
#include "base/errors.h"
#include "base/logging.h"
#include "base/task_util/default_task_runner_factory.h"
#include "base/types.h"
#include "common/buffer.h"
#include "common/codec_constants.h"
#include "common/looper.h"
#include "common/media_errors.h"
#include "common/message.h"

namespace avp {
namespace {

// Desired number of v4l2 video buffers to allocate.
constexpr uint32_t kNumVideoBuffers = 4;

// Timeout in milliseconds v4l2_thread_ blocks waiting for a frame from the hw.
// This value has been fine tuned. Before changing or modifying it see
constexpr int kCaptureTimeoutMs = 1000;

// The number of continuous timeouts tolerated before treated as error.
// constexpr int kContinuousTimeoutLimit = 10;

// Some drivers use rational time per frame instead of float frame rate, this
// constant k is used to convert between both: A fps -> [k/k*A] seconds/frame.
const int kFrameRatePrecision = 10000;

// Typical framerate, in fps
constexpr int kTypicalFramerate = 25;

// This list is ordered by precedence of use if no perfer color format
struct {
  uint32_t fourcc;
  VideoFrameBuffer::PixelFormat pixel_format;
  size_t num_planes;
} constexpr kSupportedFormatsAndPlanarity[] = {
    {V4L2_PIX_FMT_YUV420, VideoFrameBuffer::PixelFormat::kI420, 1},
    {V4L2_PIX_FMT_NV12, VideoFrameBuffer::PixelFormat::kNV12, 1},
    {V4L2_PIX_FMT_YUYV, VideoFrameBuffer::PixelFormat::kYUY2, 1},
    {V4L2_PIX_FMT_RGB24, VideoFrameBuffer::PixelFormat::kRGB24, 1},
    // error format
    {V4L2_PIX_FMT_Y16, VideoFrameBuffer::PixelFormat::kMJPEG, 1},
};

static void V4L2MakeFourCCString(uint32_t x, char* s) {
  s[3] = x >> 24;
  s[2] = (x >> 16) & 0xff;
  s[1] = (x >> 8) & 0xff;
  s[0] = x & 0xff;
  s[4] = '\0';
}

std::vector<uint32_t> getListOfUsableFourCcs(int32_t perferColorFormat) {
  std::vector<uint32_t> supportedFormats;
  uint32_t perferFourCc = 0;
  for (const auto& format : kSupportedFormatsAndPlanarity) {
    if (format.pixel_format ==
        static_cast<VideoFrameBuffer::PixelFormat>(perferColorFormat)) {
      perferFourCc = format.fourcc;
      // insert later
      continue;
    }
    supportedFormats.push_back(format.fourcc);
  }
  if (perferFourCc > 0) {
    supportedFormats.insert(supportedFormats.begin(), perferFourCc);
  }
  return supportedFormats;
}

uint32_t preferFormatFourCc(std::vector<v4l2_fmtdesc> deviceSupportV4L2Formats,
                            int32_t perferColorFormat) {
  const std::vector<uint32_t>& desiredFormats =
      getListOfUsableFourCcs(perferColorFormat);

  for (auto& format : desiredFormats) {
    char cc[5];
    V4L2MakeFourCCString(format, cc);
    if (std::find_if(deviceSupportV4L2Formats.begin(),
                     deviceSupportV4L2Formats.end(),
                     [format](v4l2_fmtdesc v4l2Format) {
                       return format == v4l2Format.pixelformat;
                     }) != deviceSupportV4L2Formats.end()) {
      return format;
    }
  }
  return 0;
}

void fillV4L2Format(v4l2_format* format,
                    uint32_t width,
                    uint32_t height,
                    uint32_t pixelformat) {
  memset(format, 0, sizeof(*format));
  format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format->fmt.pix.width = width;
  format->fmt.pix.height = height;
  format->fmt.pix.pixelformat = pixelformat;
}

void fillV4L2RequestBuffer(v4l2_requestbuffers* request_buffer, int count) {
  memset(request_buffer, 0, sizeof(*request_buffer));
  request_buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request_buffer->memory = V4L2_MEMORY_MMAP;
  request_buffer->count = count;
}

// Fills all parts of |buffer|.

void fillV4L2Buffer(v4l2_buffer* buffer, int index) {
  memset(buffer, 0, sizeof(*buffer));
  buffer->memory = V4L2_MEMORY_MMAP;
  buffer->index = index;
  buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
}

static void CreateVideoFrame(std::shared_ptr<VideoFrame>& frame_buffer,
                             v4l2_format* format,
                             uint8_t* data,
                             size_t size) {
  const int64_t now = Looper::getNowUs();
  // char cc[5];
  // V4L2MakeFourCCString(format->fmt.pix.pixelformat, cc);
  // LOG(LS_INFO) << "CreateVideoFrame, format: " << cc
  //              << ", width:" << format->fmt.pix.width
  //              << ", timestamp:";
  //              << ", height:" << format->fmt.pix.height << ", size:" << size

  switch (format->fmt.pix.pixelformat) {
    case V4L2_PIX_FMT_YUV420: {
      auto video_frame_buffer =
          I420Buffer::Create(format->fmt.pix.width, format->fmt.pix.height);
      memcpy(video_frame_buffer->MutableDataY(), data,
             video_frame_buffer->StrideY() * video_frame_buffer->height());
      memcpy(
          video_frame_buffer->MutableDataU(),
          data + video_frame_buffer->StrideY() * video_frame_buffer->height(),
          video_frame_buffer->StrideU() * video_frame_buffer->height() / 2);

      memcpy(
          video_frame_buffer->MutableDataV(),
          data + video_frame_buffer->StrideY() * video_frame_buffer->height() +
              video_frame_buffer->StrideU() * video_frame_buffer->height() / 2,
          video_frame_buffer->StrideV() * video_frame_buffer->height() / 2);

      frame_buffer = std::make_shared<VideoFrame>(0, video_frame_buffer, now,
                                                  std::nullopt);
      break;
    }

    case V4L2_PIX_FMT_NV12: {
      auto video_frame_buffer = NV12Buffer::Copy(
          data, format->fmt.pix.width,
          data + format->fmt.pix.width * format->fmt.pix.height,
          format->fmt.pix.width + format->fmt.pix.width % 2,
          format->fmt.pix.width, format->fmt.pix.height);
      frame_buffer = std::make_shared<VideoFrame>(0, video_frame_buffer, now,
                                                  std::nullopt);
      break;
    }

    case V4L2_PIX_FMT_YUYV: {
      auto video_frame_buffer =
          YUYVBuffer::Copy(data, format->fmt.pix.width * 2,
                           format->fmt.pix.width, format->fmt.pix.height);
      frame_buffer = std::make_shared<VideoFrame>(0, video_frame_buffer, now,
                                                  std::nullopt);
      break;
    }

    default: {
      break;
    }
  }
}

}  // namespace

std::shared_ptr<V4L2VideoSource> V4L2VideoSource::Create(
    std::shared_ptr<Message> info) {
  std::string v4l2_dev;
  CHECK(info->findString("v4l2-dev", v4l2_dev));
  return std::make_shared<V4L2VideoSource>(v4l2_dev.c_str());
}

V4L2VideoSource::V4L2VideoSource(const char* device, protect_parameter)
    : task_runner_factory_(base::CreateDefaultTaskRunnerFactory()),
      task_runner_(std::make_unique<base::TaskRunner>(
          task_runner_factory_->CreateTaskRunner(
              "V4L2VideoSource",
              base::TaskRunnerFactory::Priority::NORMAL))),
      mFd(-1),
      frame_count_(0) {
  LOG(LS_INFO) << "V4L2VideoSource::V4L2VideoSource";
  mFd = ::open(device, O_RDWR);
  if (mFd < 0) {
    LOG(LS_ERROR) << "open " << device << " fail, maybe no permission";
    return;
  }

  v4l2_capability cap = {};
  if (!(doIoctl(VIDIOC_QUERYCAP, &cap) == 0 &&
        (((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
          !(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) ||
         ((cap.capabilities & V4L2_CAP_DEVICE_CAPS) &&
          (cap.device_caps & V4L2_CAP_VIDEO_CAPTURE) &&
          !(cap.device_caps & V4L2_CAP_VIDEO_OUTPUT))))) {
    LOG(LS_ERROR) << "get cap error, not v4l2 device";
    return;
  }
  mV4L2Capability = cap;

  v4l2_fmtdesc fmtdesc = {};
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  for (; doIoctl(VIDIOC_ENUM_FMT, &fmtdesc) == 0; ++fmtdesc.index) {
    char cc[5];
    V4L2MakeFourCCString(fmtdesc.pixelformat, cc);
    char description[32];
    for (int i = 0; i < 31; i++) {
      description[i] = fmtdesc.description[i];
    }
    LOG(LS_DEBUG) << "support format: " << cc << ", desc: " << description;
    mV4L2Formats.push_back(fmtdesc);
  }
  MetaData metaData;
  metaData.setInt32(kKeyWidth, 1920);
  metaData.setInt32(kKeyHeight, 1080);
  metaData.setInt32(kKeyColorFormat,
                    static_cast<int32_t>(VideoFrameBuffer::PixelFormat::kI420));
  start(&metaData);

  repeating_task_handler_ = RepeatingTaskHandle::DelayedStart(
      task_runner_->Get(), 1 * 1000 * 1000, [this]() {
        std::shared_ptr<VideoFrame> frame;
        read(frame);
        if (frame) {
          for (auto& sink : sink_pairs()) {
            sink.sink->OnFrame(frame);
          }
        }
        //   50ms, about 20fps
        return (uint64_t)(50 * 1000);
      });
}

V4L2VideoSource::~V4L2VideoSource() {
  if (mFd > 0) {
    ::close(mFd);
    mFd = -1;
  }
}

void V4L2VideoSource::AddOrUpdateSink(
    VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink,
    const VideoSinkWants& wants) {
  task_runner_->PostTask([this, sink, wants]() {
    VideoSourceBase<std::shared_ptr<VideoFrame>>::AddOrUpdateSink(sink, wants);
  });
}

void V4L2VideoSource::RemoveSink(
    VideoSinkInterface<std::shared_ptr<VideoFrame>>* sink) {
  task_runner_->PostTask([this, sink]() {
    LOG(LS_INFO) << "RemoveSink";
    VideoSourceBase::RemoveSink(sink);
  });
}

status_t V4L2VideoSource::start(MetaData* params) {
  if (mFd < 0) {
    return NO_INIT;
  }
  // TODO() reset

  // read params
  int32_t width = 0;
  CHECK(params->findInt32(kKeyWidth, &width));
  int32_t height = 0;
  CHECK(params->findInt32(kKeyHeight, &height));

  int32_t preferColorFormat = -1;
  CHECK(params->findInt32(kKeyColorFormat, &preferColorFormat));

  // 1. prefer format, 2. support formats by device, support formats in code;
  uint32_t colorFormat = preferFormatFourCc(mV4L2Formats, preferColorFormat);
  if (colorFormat == 0) {
    return ERROR_UNSUPPORTED;
  }

  char cc[5];
  V4L2MakeFourCCString(colorFormat, cc);
  LOG(LS_INFO) << "choosen pixel format is " << cc;
  mColorFormat = colorFormat;

  fillV4L2Format(&mVideoFmt, width, height, colorFormat);
  if (doIoctl(VIDIOC_S_FMT, &mVideoFmt) < 0) {
    LOG(LS_ERROR) << "failed to set video format";
    return ERROR_UNSUPPORTED;
  }
  V4L2MakeFourCCString(mVideoFmt.fmt.pix.pixelformat, cc);

  mWidth = mVideoFmt.fmt.pix.width;
  mHeight = mVideoFmt.fmt.pix.height;

  LOG(LS_INFO) << "set format: " << cc << ", res: [" << width << "x" << height
               << "] -> [" << mWidth << "x" << mHeight << "]";

  int32_t framerate = 0;
  params->findInt32(kKeyFrameRate, &framerate);

  v4l2_streamparm streamparm = {};
  streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (doIoctl(VIDIOC_G_PARM, &streamparm) >= 0) {
    if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
      streamparm.parm.capture.timeperframe.numerator = kFrameRatePrecision;
      streamparm.parm.capture.timeperframe.denominator =
          (framerate) ? (framerate * kFrameRatePrecision)
                      : (kTypicalFramerate * kFrameRatePrecision);

      if (doIoctl(VIDIOC_S_PARM, &streamparm) < 0) {
        LOG(LS_ERROR) << "fail to set camera framerate";
        return ERROR_UNSUPPORTED;
      }
      LOG(LS_INFO) << "camera driver framerate: "
                   << streamparm.parm.capture.timeperframe.denominator << "/"
                   << streamparm.parm.capture.timeperframe.numerator;
    }
  }

  if (!startStream()) {
    return UNKNOWN_ERROR;
  }

  return OK;
}

bool V4L2VideoSource::mapAndQueueBuffer(int index) {
  v4l2_buffer buffer;
  fillV4L2Buffer(&buffer, index);
  if (doIoctl(VIDIOC_QUERYBUF, &buffer) < 0) {
    LOG(LS_ERROR) << "error querying status of a MMAP V4L2 buffer";
    return false;
  }

  constexpr int kFlags = PROT_READ | PROT_WRITE;
  void* const start =
      ::mmap(nullptr, buffer.length, kFlags, MAP_SHARED, mFd, buffer.m.offset);

  if (start == MAP_FAILED) {
    LOG(LS_ERROR) << "Error mmap()ing a V4L2 buffer into userspace";
    return false;
  }

  V4L2Buffer v4lBuffer = {};
  v4lBuffer.data = static_cast<uint8_t*>(start);
  v4lBuffer.size = buffer.length;
  mBuffers.push_back(v4lBuffer);

  // enqueue the buffer
  if (doIoctl(VIDIOC_QBUF, &buffer) < 0) {
    LOG(LS_ERROR) << "Error enqueuing a V4L2 buffer back into the driver";
    return false;
  }

  return true;
}

bool V4L2VideoSource::startStream() {
  v4l2_requestbuffers r_buffer;
  fillV4L2RequestBuffer(&r_buffer, kNumVideoBuffers);
  if (doIoctl(VIDIOC_REQBUFS, &r_buffer) < 0) {
    LOG(LS_ERROR) << "failed to mmap buffers from V4L2";
    return false;
  }

  for (unsigned int i = 0; i < r_buffer.count; ++i) {
    if (!mapAndQueueBuffer(i)) {
      LOG(LS_ERROR) << "Allocate buffer failed";
      return false;
    }
  }

  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (doIoctl(VIDIOC_STREAMON, &capture_type) < 0) {
    LOG(LS_ERROR) << "VIDIOC_STREAMON failed";
    return false;
  }

  return true;
}

status_t V4L2VideoSource::stop() {
  stopStream();
  return OK;
}

bool V4L2VideoSource::stopStream() {
  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (doIoctl(VIDIOC_STREAMOFF, &capture_type) < 0) {
    LOG(LS_ERROR) << "VIDIOC_STREAMOFF failed";

    return false;
  }

  v4l2_requestbuffers r_buffer;
  fillV4L2RequestBuffer(&r_buffer, 0);
  if (doIoctl(VIDIOC_REQBUFS, &r_buffer) < 0) {
    LOG(LS_ERROR) << "Failed to VIDIOC_REQBUFS with count = 0";

    return false;
  }
  return true;
}

status_t V4L2VideoSource::read(std::shared_ptr<VideoFrame>& buffer) {
  pollfd pfd = {};
  pfd.fd = mFd;
  pfd.events = POLLIN;
  const int result = ::poll(&pfd, 1, kCaptureTimeoutMs);
  if (result < 0) {
    LOG(LS_ERROR) << "poll failed " << result;
    return result;
  }

  if (result == 0) {
    return ERROR_RETRY;
  }
  //  LOG(LS_VERBOSE) << "pfd.events:" << pfd.events
  //                  << ", pfd.revents:" << pfd.revents;

  if (pfd.revents & POLLIN) {
    v4l2_buffer v4lBuffer;
    fillV4L2Buffer(&v4lBuffer, 0);

    if (doIoctl(VIDIOC_DQBUF, &v4lBuffer) < 0) {
      LOG(LS_ERROR) << "failed to dequeue capture buffer";
      return UNKNOWN_ERROR;
    }

    CreateVideoFrame(buffer, &mVideoFmt, mBuffers[v4lBuffer.index].data,
                     v4lBuffer.bytesused);

    if (buffer != nullptr) {
      // TODO(youfa) use now us or v4l2 timestamp
      // const int64_t now = Looper::getNowUs();
      // LOG(LS_INFO) << "now:" << now
      //             << ", v4lBuffer.timestamp:" << v4lBuffer.timestamp.tv_sec
      //             << "," << v4lBuffer.timestamp.tv_usec << ", ts:"
      //             << v4lBuffer.timestamp.tv_sec * 1000000 +
      //                    v4lBuffer.timestamp.tv_usec;
      // buffer->set_timestamp_us(now);
      buffer->set_id(frame_count_++);
    }

    if (doIoctl(VIDIOC_QBUF, &v4lBuffer) < 0) {
      LOG(LS_ERROR) << "failed to enqueue capture buffer";
      return UNKNOWN_ERROR;
    }

    return OK;
  }
  return OK;
}

status_t V4L2VideoSource::pause() {
  return ERROR_RETRY;
}

int V4L2VideoSource::doIoctl(int request, void* argp) {
  return ::ioctl(mFd, request, argp);
}

}  // namespace avp
