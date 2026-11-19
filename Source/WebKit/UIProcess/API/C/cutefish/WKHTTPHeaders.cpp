// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKHTTPHeaders.h"

#include "APIURLRequest.h"
#include "APIURLResponse.h"
#include "WKAPICast.h"
#include <wtf/text/WTFString.h>

WKURLRequestRef WKURLRequestCopySettingHTTPHeaderField(WKURLRequestRef requestRef, const char* name, const char* value)
{
    WebCore::ResourceRequest request = WebKit::toImpl(requestRef)->resourceRequest();
    request.setHTTPHeaderField(String::fromUTF8(name), String::fromUTF8(value));
    return WebKit::toAPILeakingRef(API::URLRequest::create(request));
}

WKStringRef WKURLResponseCopyHTTPHeaderField(WKURLResponseRef responseRef, const char* name)
{
    String value = WebKit::toImpl(responseRef)->resourceResponse().httpHeaderField(String::fromUTF8(name));
    return value.isNull() ? nullptr : WebKit::toCopiedAPI(value);
}
