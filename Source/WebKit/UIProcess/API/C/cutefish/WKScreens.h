// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKScreens_h
#define WKScreens_h

#include <WebKit/WKBase.h>
#include <WebKit/WKGeometry.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// A screen as pages see it (screen.width, screen.availHeight, the colour
// depth, resolution media queries), in logical pixels with the origin at
// the top left of the desktop. identifier is non-zero.
typedef struct WKScreenInfo {
    uint32_t identifier;
    WKRect frame;
    WKRect availableFrame;
    int32_t depth;
    int32_t depthPerComponent;
    int32_t widthMillimeters;
    int32_t heightMillimeters;
    double dpi;
} WKScreenInfo;

// The desktop's screens, which every web process learns, now and whenever
// they change.
WK_EXPORT void WKSetScreens(const WKScreenInfo* screens, size_t count, uint32_t primaryIdentifier);

#ifdef __cplusplus
}
#endif

#endif // WKScreens_h
