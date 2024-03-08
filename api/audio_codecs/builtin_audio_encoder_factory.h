/*
 * builtin_audio_encoder_factory.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef BUILTIN_AUDIO_ENCODER_FACTORY_H
#define BUILTIN_AUDIO_ENCODER_FACTORY_H

#include "api/audio_codecs/audio_encoder_factory.h"

namespace ave {
std::unique_ptr<AudioEncoderFactory> CreateBuiltinAudioEncoderFactory();

}

#endif /* !BUILTIN_AUDIO_ENCODER_FACTORY_H */
