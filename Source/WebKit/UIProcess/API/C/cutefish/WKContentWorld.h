// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKContentWorld_h
#define WKContentWorld_h

#include <WebKit/WKBase.h>
#include <WebKit/WKPage.h>
#include <WebKit/WKUserContentControllerRef.h>
#include <WebKit/WKUserScriptRef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Scripts in different worlds share the page's document but not their
// globals. A world is named by a string: null is the page's own world, an
// empty string the default world for the embedder's scripts, and any other
// name a world shared by everything that uses that name.
WK_EXPORT WKUserScriptRef WKUserScriptCreateWithSourceInWorld(WKStringRef source, _WKUserScriptInjectionTime, bool forMainFrameOnly, WKStringRef world);
WK_EXPORT void WKUserContentControllerAddScriptMessageHandlerInWorld(WKUserContentControllerRef, WKStringRef name, WKStringRef world, WKScriptMessageHandlerCallback, const void* context);
WK_EXPORT void WKPageEvaluateJavaScriptInFrameInWorld(WKPageRef, WKFrameInfoRef, WKStringRef script, WKStringRef world, void* context, WKPageEvaluateJavaScriptFunction);
WK_EXPORT void WKPageCallAsyncJavaScriptInWorld(WKPageRef, WKStringRef script, WKDictionaryRef arguments, WKFrameInfoRef, WKStringRef world, void* context, WKPageEvaluateJavaScriptFunction);

#ifdef __cplusplus
}
#endif

#endif // WKContentWorld_h
