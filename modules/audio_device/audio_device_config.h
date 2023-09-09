/*
 * audio_device_config.h
 * Copyright (C) 2023 youfa <vsyfar@gmail.com>
 *
 * Distributed under terms of the GPLv2 license.
 */

#ifndef AUDIO_DEVICE_CONFIG_H
#define AUDIO_DEVICE_CONFIG_H

// Enumerators
//
enum { GET_MIC_VOLUME_INTERVAL_MS = 1000 };

// Platform specifics
//
#if defined(_WIN32)
#if (_MSC_VER >= 1400)
#if !defined(WEBRTC_DUMMY_FILE_DEVICES)
// Windows Core Audio is the default audio layer in Windows.
// Only supported for VS 2005 and higher.
#define WEBRTC_WINDOWS_CORE_AUDIO_BUILD
#endif
#endif
#endif

#endif /* !AUDIO_DEVICE_CONFIG_H */
