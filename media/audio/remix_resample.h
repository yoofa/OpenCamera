/*
 * remix_resample.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef REMIX_RESAMPLE_H
#define REMIX_RESAMPLE_H

#include "base/types.h"

namespace ave {

void RemixAndResample(const int16_t* src,
                      uint32_t src_channels,
                      uint32_t src_sample_rate,
                      uint32_t samples_per_channel,
                      int16_t* dst,
                      uint32_t dst_channels,
                      uint32_t dst_sample_rate);

}  // namespace ave

#endif /* !REMIX_RESAMPLE_H */
