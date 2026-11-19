// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKHTTPHeaders_h
#define WKHTTPHeaders_h

#include <WebKit/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

// A copy of the request with the header field set, replacing any value it
// had.
WK_EXPORT WKURLRequestRef WKURLRequestCopySettingHTTPHeaderField(WKURLRequestRef, const char* name, const char* value);

// The value of the response's header field, or null when it has none.
WK_EXPORT WKStringRef WKURLResponseCopyHTTPHeaderField(WKURLResponseRef, const char* name);

#ifdef __cplusplus
}
#endif

#endif // WKHTTPHeaders_h
