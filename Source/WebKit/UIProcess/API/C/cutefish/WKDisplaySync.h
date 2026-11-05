// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKDisplaySync_h
#define WKDisplaySync_h

#include <WebKit/WKBase.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// The display's refreshes as the embedder sees them, which pace the pages'
// rendering and animations. While started, the embedder calls
// WKDisplaySyncFired on the main thread once per refresh, as its windows
// present frames; stopped, it calls nothing. Without a client the pages
// keep a timer of their own.
typedef struct WKDisplaySyncClient {
    void* context;
    void (*start)(void* context);
    void (*stop)(void* context);
    // Refreshes per second, in thousandths.
    unsigned refreshRate;
} WKDisplaySyncClient;

// Set before the first view is created.
WK_EXPORT void WKDisplaySyncSetClient(const WKDisplaySyncClient*);
WK_EXPORT void WKDisplaySyncFired(void);

#ifdef __cplusplus
}
#endif

#endif // WKDisplaySync_h
