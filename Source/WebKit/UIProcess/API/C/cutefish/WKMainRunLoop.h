// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKMainRunLoop_h
#define WKMainRunLoop_h

#include <WebKit/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

// For an embedder that runs its own main loop instead of WKRunLoopRunMain().
// The callback may run on any thread and while WebKit holds locks: it must
// only wake the embedder's main loop, which then calls WKMainRunLoopIterate().
typedef void (*WKMainRunLoopWakeUpCallback)(void* userData);
WK_EXPORT void WKMainRunLoopSetWakeUpCallback(WKMainRunLoopWakeUpCallback, void* userData);

// Runs the work that is due on the main thread without blocking. Returns the
// seconds until it needs to run again, or a negative value when only a wake-up
// can bring new work.
WK_EXPORT double WKMainRunLoopIterate(void);

#ifdef __cplusplus
}
#endif

#endif /* WKMainRunLoop_h */
