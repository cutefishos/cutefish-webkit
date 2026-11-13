// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKMainRunLoop_h
#define WKMainRunLoop_h

#include <WebKit/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

// For an embedder that runs its own main loop instead of WKRunLoopRunMain().
// The descriptor becomes readable when WebKit has work for the main thread;
// the embedder then calls WKMainRunLoopIterate(). Both run on the main thread.
WK_EXPORT int WKMainRunLoopGetDescriptor(void);

// Runs the work that is due on the main thread without blocking. Returns the
// seconds until it needs to run again, or a negative value when only the
// descriptor can bring new work.
WK_EXPORT double WKMainRunLoopIterate(void);

// Makes the descriptor readable. Safe to call from any thread.
WK_EXPORT void WKMainRunLoopWakeUp(void);

#ifdef __cplusplus
}
#endif

#endif /* WKMainRunLoop_h */
