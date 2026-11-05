// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKURLSchemeHandler_h
#define WKURLSchemeHandler_h

#include <WebKit/WKBase.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct OpaqueWKURLSchemeTask* WKURLSchemeTaskRef;

// A task starts with a request and ends with didFinish, didFail or the
// stop callback; after stop, its other calls do nothing. Retain the task
// to answer it later.
typedef void (*WKURLSchemeHandlerStartTaskCallback)(WKURLSchemeTaskRef, const void* clientInfo);
typedef void (*WKURLSchemeHandlerStopTaskCallback)(WKURLSchemeTaskRef, const void* clientInfo);

typedef struct WKURLSchemeHandlerClientBase {
    int version;
    const void* clientInfo;
} WKURLSchemeHandlerClientBase;

typedef struct WKURLSchemeHandlerClientV0 {
    WKURLSchemeHandlerClientBase base;

    // version 0
    WKURLSchemeHandlerStartTaskCallback startTask;
    WKURLSchemeHandlerStopTaskCallback stopTask;
} WKURLSchemeHandlerClientV0;

// Pages made from the configuration load URLs of the scheme through the
// client.
WK_EXPORT void WKPageConfigurationSetURLSchemeHandler(WKPageConfigurationRef, const char* scheme, const WKURLSchemeHandlerClientBase*);

WK_EXPORT WKURLRequestRef WKURLSchemeTaskCopyRequest(WKURLSchemeTaskRef);

// statusCode is set on the response when it is not zero.
WK_EXPORT void WKURLSchemeTaskDidReceiveResponse(WKURLSchemeTaskRef, const char* mimeType, int64_t expectedLength, const char* textEncodingName, int statusCode);
WK_EXPORT void WKURLSchemeTaskDidReceiveData(WKURLSchemeTaskRef, const void* bytes, size_t length);
WK_EXPORT void WKURLSchemeTaskDidFinish(WKURLSchemeTaskRef);
WK_EXPORT void WKURLSchemeTaskDidFail(WKURLSchemeTaskRef, const char* domain, int code, const char* localizedDescription);

#ifdef __cplusplus
}
#endif

#endif /* WKURLSchemeHandler_h */
