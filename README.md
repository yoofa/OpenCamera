# OpenCamera
A cross-platform surveillance camera client engineered to flawlessly capture both audio and video, streaming the content using diverse protocols.
Applicable in indoor surveillance, visual doorbells, dashcams, and other scenarios.


## How to use

### 1. install google [depot_tools](https://www.chromium.org/developers/how-tos/install-depot-tools/)

I use depot_tools to download and build OpenCamera.

### 2. download

```
mkdir OpenCamera && cd OpenCamera
gclient config --unmanaged git@github.com:vsyf/OpenCamera.git --name src
gclient sync
cd src
```

### 2. build

```
gn gen out/Default --export-compile-commands
ninja -C out/Default
```

### 3. run

`sudo ./out/Default/DyrachYO -c data/open_camera.ini`


## Features

### Audio and Video Interfaces

- [x] Linux V4L2
- [x] Linux ALSA
- [] Linux PulseAudio
- [] Android Camera
- [] Android Audio

### Codecs
- [x] FDK-AAC

- [x] OpenH264
- [] Android MediaCodec


### Streaming and Storage

- [x] RTSP
- [] RTMP
- [] HLS
- [] Local File Storage
- [] Samba

### Configure Client

- [x] ONVIF (ongoing)
- [] HTTP

### Audio and Video Processing
- [] Motion Detect

