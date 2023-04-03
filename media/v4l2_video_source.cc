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

std::vector<uint32_t> getListOfUsableFourCcs(int32_t perferColorFormat) {
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

}  // namespace

V4L2VideoSource::V4L2VideoSource(const char* device) : mFd(-1) {
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
    LOG(LS_DEBUG) << "support format: " << cc
                  << ", desc: " << fmtdesc.description;
    mV4L2Formats.push_back(fmtdesc);
  }
  //
}

V4L2VideoSource::~V4L2VideoSource() {
  if (mFd > 0) {
    ::close(mFd);
    mFd = -1;
  }
}

status_t V4L2VideoSource::start(MetaData* params) {
  if (mFd < 0) {
    return NO_INIT;
  }
  // TODO() reset

  // read params
  int32_t width = 0;
  DCHECK(params->findInt32(kKeyWidth, &width));
  int32_t height = 0;
  DCHECK(params->findInt32(kKeyHeight, &height));

  int32_t preferColorFormat = -1;
  DCHECK(params->findInt32(kKeyColorFormat, &preferColorFormat));

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
  LOG(LS_INFO) << "set format: " << cc << ", width: " << mVideoFmt.fmt.pix.width
               << ", height: " << mVideoFmt.fmt.pix.height;

  mWidth = mVideoFmt.fmt.pix.width;
  mHeight = mVideoFmt.fmt.pix.height;

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

std::shared_ptr<MetaData> V4L2VideoSource::getFormat() {
  return nullptr;
}

status_t V4L2VideoSource::read(std::shared_ptr<Buffer>& buffer,
                               const ReadOptions* options) {
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

    // TODO(youfa) change get now us to //base
    const int64_t now = Looper::getNowUs();
    buffer = std::make_shared<Buffer>(v4lBuffer.bytesused);
    memcpy(buffer->data(), mBuffers[v4lBuffer.index].data, v4lBuffer.bytesused);
    buffer->meta()->setInt64("timeUs", now);
    buffer->meta()->setInt32("width", mWidth);
    buffer->meta()->setInt32("height", mHeight);

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

status_t V4L2VideoSource::setBuffers(const std::vector<Buffer*>&) {
  return OK;
}

status_t V4L2VideoSource::setStopTimeUs(int64_t) {
  return OK;
}

int V4L2VideoSource::doIoctl(int request, void* argp) {
  return ::ioctl(mFd, request, argp);
}
class V4l2VideoSourceFactory : public VideoSourceFactory {
 public:
  V4l2VideoSourceFactory() = default;
  virtual ~V4l2VideoSourceFactory() = default;

  std::unique_ptr<MediaSource> CreateVideoSource(
      std::shared_ptr<Message> info) {
    // TODO
    std::string v4l2_dev;
    DCHECK(info->findString("v4l2-dev", v4l2_dev));
    return std::make_unique<V4L2VideoSource>(v4l2_dev.c_str());
  }

 private:
  /* data */
};

std::unique_ptr<VideoSourceFactory> CreateV4l2VideoSourceFactory() {
  // TODO
  return std::make_unique<V4l2VideoSourceFactory>();
}

}  // namespace avp
