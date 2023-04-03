/*
 * h264_file_source.h
 * Copyright (C) 2022 youfa.song <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef H264_FILE_SOURCE_H
#define H264_FILE_SOURCE_H

#include <memory>

#include "common/media_source.h"
#include "common/meta_data.h"

namespace avp {

#define FRAME_MAX_SIZE (1024 * 500)

class H264FileSource : public MediaSource {
 public:
  H264FileSource(const char* path);
  virtual ~H264FileSource();

  status_t start(MetaData* params = nullptr) override;

  status_t stop() override;

  std::shared_ptr<MetaData> getFormat() override;

  status_t read(std::shared_ptr<Buffer>& buffer,
                const ReadOptions* options = nullptr) override;

 private:
  std::shared_ptr<MetaData> meta_;
  int fd_;
  std::unique_ptr<uint8_t[]> buf_;

  AVP_DISALLOW_COPY_AND_ASSIGN(H264FileSource);
};

}  // namespace avp

#endif /* !H264_FILE_SOURCE_H */
