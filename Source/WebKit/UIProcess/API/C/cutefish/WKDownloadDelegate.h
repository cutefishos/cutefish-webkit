// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKDownloadDelegate_h
#define WKDownloadDelegate_h

#include <WebKit/WKBase.h>
#include <WebKit/WKDownloadClient.h>

#ifdef __cplusplus
extern "C" {
#endif

// A question the download waits on; answer it exactly once, at any time.
typedef struct OpaqueWKDownloadListener* WKDownloadListenerRef;

typedef void (*WKDownloadDelegateDecideDestinationCallback)(WKDownloadRef, WKURLResponseRef, WKStringRef suggestedFilename, WKDownloadListenerRef, const void* clientInfo);
typedef void (*WKDownloadDelegateWillPerformHTTPRedirectionCallback)(WKDownloadRef, WKURLResponseRef, WKURLRequestRef newRequest, WKDownloadListenerRef, const void* clientInfo);

typedef struct WKDownloadDelegateBase {
    int version;
    const void* clientInfo;
} WKDownloadDelegateBase;

typedef struct WKDownloadDelegateV0 {
    WKDownloadDelegateBase base;

    // version 0
    WKDownloadDelegateWillPerformHTTPRedirectionCallback willPerformHTTPRedirection;
    WKDownloadDidReceiveAuthenticationChallenge didReceiveAuthenticationChallenge;
    WKDownloadDelegateDecideDestinationCallback decideDestination;
    WKDownloadDidWriteData didWriteData;
    WKDownloadDidFinish didFinish;
    WKDownloadDidFailWithError didFailWithError;
} WKDownloadDelegateV0;

// Like WKDownloadSetClient, but the destination and redirects are decided
// later through a listener.
WK_EXPORT void WKDownloadSetDelegate(WKDownloadRef, const WKDownloadDelegateBase*);

// A null or empty path cancels the download.
WK_EXPORT void WKDownloadListenerChooseDestination(WKDownloadListenerRef, const char* path, bool allowOverwrite);
WK_EXPORT void WKDownloadListenerAllowRedirect(WKDownloadListenerRef, bool allow);

// Downloads the request on behalf of the page; the callback receives the
// download once it starts, with no delegate set yet.
typedef void (*WKPageDownloadStartedCallback)(WKDownloadRef, void* context);
WK_EXPORT void WKPageDownloadRequest(WKPageRef, WKURLRequestRef, void* context, WKPageDownloadStartedCallback);

#ifdef __cplusplus
}
#endif

#endif /* WKDownloadDelegate_h */
