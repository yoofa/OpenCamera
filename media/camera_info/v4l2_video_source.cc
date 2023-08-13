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

#include "base/checks.h"
#include "base/errors.h"
#include "base/logging.h"
#include "common/buffer.h"
#include "common/codec_constants.h"
#include "common/looper.h"
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
  int32_t pixel_format;
  size_t num_planes;
} constexpr kSupportedFormatsAndPlanarity[] = {
    {V4L2_PIX_FMT_YUV420, COLOR_FormatYUV420Planar, 1},
    {V4L2_PIX_FMT_NV12, COLOR_FormatYUV420SemiPlanar, 1},
    {V4L2_PIX_FMT_YUYV, COLOR_FormatYUV422PackedPlanar, 1},
    {V4L2_PIX_FMT_RGB24, COLOR_Format24bitRGB888, 1},
    {V4L2_PIX_FMT_Y16, COLOR_FormatL16, 1},
};

static void V4L2MakeFourCCString(uint32_t x, char* s) {
  s[3] = x >> 24;
  s[2] = (x >> 16) & 0xff;
  s[1] = (x >> 8) & 0xff;
  s[0] = x & 0xff;
  s[4] = '\0';
}

std::vector<uint32_t> GetListOfUsableFourCcs(int32_t perferColorFormat) {
  std::vector<uint32_t> supportedFormats;
  uint32_t perferFourCc = 0;
  for (const auto& format : kSupportedFormatsAndPlanarity) {
    if (format.pixel_format == perferColorFormat) {
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

uint32_t PreferFormatFourCc(std::vector<v4l2_fmtdesc> deviceSupportV4L2Formats,
                            int32_t perferColorFormat) {
  const std::vector<uint32_t>& desiredFormats =
      GetListOfUsableFourCcs(perferColorFormat);

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

void FillV4L2Format(v4l2_format* format,
                    uint32_t width,
                    uint32_t height,
                    uint32_t pixelformat) {
  memset(format, 0, sizeof(*format));
  format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format->fmt.pix.width = width;
  format->fmt.pix.height = height;
  format->fmt.pix.pixelformat = pixelformat;
}

void FillV4L2RequestBuffer(v4l2_requestbuffers* request_buffer, int count) {
  memset(request_buffer, 0, sizeof(*request_buffer));
  request_buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request_buffer->memory = V4L2_MEMORY_MMAP;
  request_buffer->count = count;
}

// Fills all parts of |buffer|.

void FillV4L2Buffer(v4l2_buffer* buffer, int index) {
  memset(buffer, 0, sizeof(*buffer));
  buffer->memory = V4L2_MEMORY_MMAP;
  buffer->index = index;
  buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
}

}  // namespace

V4L2VideoSourceForCameraInfo::V4L2VideoSourceForCameraInfo(const char* device)
    : fd_(-1) {
  fd_ = ::open(device, O_RDWR);
  if (fd_ < 0) {
    LOG(LS_ERROR) << "open " << device << " fail, maybe no permission";
    return;
  }

  v4l2_capability cap = {};
  if (!(DoIoctl(VIDIOC_QUERYCAP, &cap) == 0 &&
        (((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
          !(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) ||
         ((cap.capabilities & V4L2_CAP_DEVICE_CAPS) &&
          (cap.device_caps & V4L2_CAP_VIDEO_CAPTURE) &&
          !(cap.device_caps & V4L2_CAP_VIDEO_OUTPUT))))) {
    LOG(LS_ERROR) << "get cap error, not v4l2 device";
    return;
  }
  v4l2_capability_ = cap;

  v4l2_fmtdesc fmtdesc = {};
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  for (; DoIoctl(VIDIOC_ENUM_FMT, &fmtdesc) == 0; ++fmtdesc.index) {
    char cc[5];
    V4L2MakeFourCCString(fmtdesc.pixelformat, cc);
    LOG(LS_DEBUG) << "support format: " << cc
                  << ", desc: " << fmtdesc.description;
    mV4L2Formats.push_back(fmtdesc);
  }
  //
}

V4L2VideoSourceForCameraInfo::~V4L2VideoSourceForCameraInfo() {
  if (fd_ > 0) {
    ::close(fd_);
    fd_ = -1;
  }
}

status_t V4L2VideoSourceForCameraInfo::start(MetaData* params) {
  if (fd_ < 0) {
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
  uint32_t colorFormat = PreferFormatFourCc(mV4L2Formats, preferColorFormat);
  if (colorFormat == 0) {
    return ERROR_UNSUPPORTED;
  }

  char cc[5];
  V4L2MakeFourCCString(colorFormat, cc);
  LOG(LS_INFO) << "choosen pixel format is " << cc;
  color_format_ = colorFormat;

  FillV4L2Format(&video_format_, width, height, colorFormat);
  if (DoIoctl(VIDIOC_S_FMT, &video_format_) < 0) {
    LOG(LS_ERROR) << "failed to set video format";
    return ERROR_UNSUPPORTED;
  }
  V4L2MakeFourCCString(video_format_.fmt.pix.pixelformat, cc);
  LOG(LS_INFO) << "set format: " << cc
               << ", width: " << video_format_.fmt.pix.width
               << ", height: " << video_format_.fmt.pix.height;

  width_ = video_format_.fmt.pix.width;
  height_ = video_format_.fmt.pix.height;

  int32_t framerate = 0;
  params->findInt32(kKeyFrameRate, &framerate);

  v4l2_streamparm streamparm = {};
  streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(VIDIOC_G_PARM, &streamparm) >= 0) {
    if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
      streamparm.parm.capture.timeperframe.numerator = kFrameRatePrecision;
      streamparm.parm.capture.timeperframe.denominator =
          (framerate) ? (framerate * kFrameRatePrecision)
                      : (kTypicalFramerate * kFrameRatePrecision);

      if (DoIoctl(VIDIOC_S_PARM, &streamparm) < 0) {
        LOG(LS_ERROR) << "fail to set camera framerate";
        return ERROR_UNSUPPORTED;
      }
      LOG(LS_INFO) << "camera driver framerate: "
                   << streamparm.parm.capture.timeperframe.denominator << "/"
                   << streamparm.parm.capture.timeperframe.numerator;
    }
  }

  if (!StartStream()) {
    return UNKNOWN_ERROR;
  }

  return OK;
}

bool V4L2VideoSourceForCameraInfo::MapAndQueueBuffer(int index) {
  v4l2_buffer buffer;
  FillV4L2Buffer(&buffer, index);
  if (DoIoctl(VIDIOC_QUERYBUF, &buffer) < 0) {
    LOG(LS_ERROR) << "error querying status of a MMAP V4L2 buffer";
    return false;
  }

  constexpr int kFlags = PROT_READ | PROT_WRITE;
  void* const start =
      ::mmap(nullptr, buffer.length, kFlags, MAP_SHARED, fd_, buffer.m.offset);

  if (start == MAP_FAILED) {
    LOG(LS_ERROR) << "Error mmap()ing a V4L2 buffer into userspace";
    return false;
  }

  V4L2Buffer v4lBuffer = {};
  v4lBuffer.data = static_cast<uint8_t*>(start);
  v4lBuffer.size = buffer.length;
  buffers_.push_back(v4lBuffer);

  // enqueue the buffer
  if (DoIoctl(VIDIOC_QBUF, &buffer) < 0) {
    LOG(LS_ERROR) << "Error enqueuing a V4L2 buffer back into the driver";
    return false;
  }

  return true;
}

bool V4L2VideoSourceForCameraInfo::StartStream() {
  v4l2_requestbuffers r_buffer;
  FillV4L2RequestBuffer(&r_buffer, kNumVideoBuffers);
  if (DoIoctl(VIDIOC_REQBUFS, &r_buffer) < 0) {
    LOG(LS_ERROR) << "failed to mmap buffers from V4L2";
    return false;
  }

  for (unsigned int i = 0; i < r_buffer.count; ++i) {
    if (!MapAndQueueBuffer(i)) {
      LOG(LS_ERROR) << "Allocate buffer failed";
      return false;
    }
  }

  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(VIDIOC_STREAMON, &capture_type) < 0) {
    LOG(LS_ERROR) << "VIDIOC_STREAMON failed";
    return false;
  }

  return true;
}

status_t V4L2VideoSourceForCameraInfo::stop() {
  StopStream();
  return OK;
}

bool V4L2VideoSourceForCameraInfo::StopStream() {
  v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (DoIoctl(VIDIOC_STREAMOFF, &capture_type) < 0) {
    LOG(LS_ERROR) << "VIDIOC_STREAMOFF failed";

    return false;
  }

  v4l2_requestbuffers r_buffer;
  FillV4L2RequestBuffer(&r_buffer, 0);
  if (DoIoctl(VIDIOC_REQBUFS, &r_buffer) < 0) {
    LOG(LS_ERROR) << "Failed to VIDIOC_REQBUFS with count = 0";

    return false;
  }
  return true;
}

std::shared_ptr<MetaData> V4L2VideoSourceForCameraInfo::getFormat() {
  return nullptr;
}

status_t V4L2VideoSourceForCameraInfo::read(std::shared_ptr<Buffer>& buffer,
                                            const ReadOptions* options) {
  pollfd pfd = {};
  pfd.fd = fd_;
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
    FillV4L2Buffer(&v4lBuffer, 0);

    if (DoIoctl(VIDIOC_DQBUF, &v4lBuffer) < 0) {
      LOG(LS_ERROR) << "failed to dequeue capture buffer";
      return UNKNOWN_ERROR;
    }

    // TODO(youfa) change get now us to //base
    const int64_t now = Looper::getNowUs();
    buffer = std::make_shared<Buffer>(v4lBuffer.bytesused);
    memcpy(buffer->data(), buffers_[v4lBuffer.index].data, v4lBuffer.bytesused);
    buffer->meta()->setInt64("timeUs", now);
    buffer->meta()->setInt32("width", width_);
    buffer->meta()->setInt32("height", height_);

    if (DoIoctl(VIDIOC_QBUF, &v4lBuffer) < 0) {
      LOG(LS_ERROR) << "failed to enqueue capture buffer";
      return UNKNOWN_ERROR;
    }

    return OK;
  }
  return OK;
}

status_t V4L2VideoSourceForCameraInfo::pause() {
  return ERROR_RETRY;
}

status_t V4L2VideoSourceForCameraInfo::setBuffers(const std::vector<Buffer*>&) {
  return OK;
}

status_t V4L2VideoSourceForCameraInfo::setStopTimeUs(int64_t) {
  return OK;
}

int V4L2VideoSourceForCameraInfo::DoIoctl(int request, void* argp) {
  return ::ioctl(fd_, request, argp);
}
class V4l2VideoSourceFactory : public VideoSourceFactory {
 public:
  V4l2VideoSourceFactory() = default;
  virtual ~V4l2VideoSourceFactory() = default;

  std::unique_ptr<MediaSource> CreateVideoSource(
      std::shared_ptr<Message> info) {
    // TODO
    std::string v4l2_dev;
    CHECK(info->findString("v4l2-dev", v4l2_dev));
    return std::make_unique<V4L2VideoSourceForCameraInfo>(v4l2_dev.c_str());
  }

 private:
  /* data */
};

std::unique_ptr<VideoSourceFactory> CreateV4l2VideoSourceFactory() {
  // TODO
  return std::make_unique<V4l2VideoSourceFactory>();
}

}  // namespace avp
