// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKPasteboard_h
#define WKPasteboard_h

#include <WebKit/WKBase.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types are MIME types ("text/plain", "text/html", "text/uri-list",
// "image/png", ...); pages may also write types of their own.
typedef void (*WKPasteboardAddTypeFunction)(const char* type, void* context);
typedef void (*WKPasteboardSetDataFunction)(const void* bytes, size_t length, void* context);

// A number that changes whenever anyone writes the system pasteboard.
typedef int64_t (*WKPasteboardChangeCountCallback)(const void* clientInfo);
// Calls add once for each type on the pasteboard, best first.
typedef void (*WKPasteboardCopyTypesCallback)(WKPasteboardAddTypeFunction add, void* context, const void* clientInfo);
// Calls set with the bytes of the type, or not at all when it is missing.
typedef void (*WKPasteboardCopyDataCallback)(const char* type, WKPasteboardSetDataFunction set, void* context, const void* clientInfo);
// Replaces the pasteboard's contents; count is zero to clear it.
typedef void (*WKPasteboardWriteCallback)(size_t count, const char* const* types, const void* const* bytes, const size_t* lengths, const void* clientInfo);

typedef struct WKPasteboardClientBase {
    int version;
    const void* clientInfo;
} WKPasteboardClientBase;

typedef struct WKPasteboardClientV0 {
    WKPasteboardClientBase base;

    // version 0
    WKPasteboardChangeCountCallback changeCount;
    WKPasteboardCopyTypesCallback copyTypes;
    WKPasteboardCopyDataCallback copyData;
    WKPasteboardWriteCallback write;
} WKPasteboardClientV0;

// Pages copy to and paste from the client's pasteboard instead of one kept
// inside the process. The callbacks run on the main thread.
WK_EXPORT void WKPasteboardSetClient(const WKPasteboardClientBase*);

#ifdef __cplusplus
}
#endif

#endif /* WKPasteboard_h */
