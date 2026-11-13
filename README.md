# Cutefish WebKit

The web engine of the Cutefish desktop: a WebKit port based on WPE WebKit
2.54.2. WebCore, JavaScriptCore and the multi-process WebKit model are
upstream WebKit on GLib's event loop, as in GTK and WPE; the port adds the
curl network backend, Skia rendering into DMA-BUF buffers and a C API that
hands those frames to the embedding application. PDF documents open in
WebKit's bundled PDF.js viewer. Video, audio, Media Source Extensions, Web
Audio and WebCodecs run on GStreamer, with hardware decoding where a VA-API
decoder is installed. Web processes run in WebKit's bubblewrap sandbox, with
a seccomp filter and a filtered session bus; `WKContextSandbox.h` turns it off
or lets an application add paths to it.

WebGL, WebRTC, encrypted media, accessibility and the inspector are not built
yet.

## Dependencies

### Debian/Ubuntu

```shell
sudo apt install cmake ninja-build gperf perl python3 ruby unifdef pkg-config libglib2.0-dev \
    libcurl4-openssl-dev libssl-dev libpsl-dev libicu-dev libxml2-dev libxslt1-dev libsqlite3-dev \
    libegl-dev libgles-dev libepoxy-dev libgbm-dev libdrm-dev libxkbcommon-dev \
    libfontconfig-dev libfreetype-dev libharfbuzz-dev liblcms2-dev zlib1g-dev \
    libjpeg-dev libpng-dev libwebp-dev libavif-dev libjxl-dev libwoff-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libseccomp-dev bubblewrap xdg-dbus-proxy
```

At run time the sandbox needs bubblewrap, xdg-dbus-proxy and unprivileged
user namespaces. Media needs GStreamer's base and good plugin sets; the bad
set, gst-libav and the GL plugin add codecs, VA-API decoding and video frames
that stay on the GPU.

## Build

```shell
cmake -S . -B build -G Ninja -DPORT=Cutefish -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

A full build takes about 13 CPU hours; allow 2 GB of memory per job.

## Use

Applications link `libcutefish-webkit`, which holds both WebKit's and
JavaScriptCore's C APIs, through pkg-config:

```shell
pkg-config --cflags --libs cutefish-webkit
```

The port's own API is in `<WebKit/WKView.h>`, `<WebKit/WKViewClient.h>` and
`<WebKit/WKContextSandbox.h>`.
The web and network processes are installed in `libexec/cutefish-webkit/`.

The tree is upstream's `wpewebkit-2.54.2` sources with the port applied.

## License

WebKit's licences apply file by file, mostly BSD-2-Clause and LGPL-2.1; see
`Source/WebCore/LICENSE-APPLE`, `Source/WebCore/LICENSE-LGPL-2.1` and
`Source/JavaScriptCore/COPYING.LIB`. PDF.js, built into the library, is
Apache-2.0 (`Source/ThirdParty/pdfjs/LICENSE`). Port files derived from
WebKit code keep their original licence; files written from scratch are
BSD-3-Clause.
