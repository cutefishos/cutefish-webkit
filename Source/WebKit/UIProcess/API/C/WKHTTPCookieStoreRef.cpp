/*
 * Copyright (C) 2019-2025 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WKHTTPCookieStoreRef.h"

#include "APIHTTPCookieStore.h"
#include "WKAPICast.h"
#include <WebCore/Cookie.h>
#include <wtf/text/CString.h>

WKTypeID WKHTTPCookieStoreGetTypeID()
{
    return WebKit::toAPI(API::HTTPCookieStore::APIType);
}

void WKHTTPCookieStoreDeleteAllCookies(WKHTTPCookieStoreRef cookieStore, void* context, WKHTTPCookieStoreDeleteAllCookiesFunction callback)
{
    protect(WebKit::toImpl(cookieStore))->deleteAllCookies([context, callback] {
        if (callback)
            callback(context);
    });
}

void WKHTTPCookieStoreSetHTTPCookieAcceptPolicy(WKHTTPCookieStoreRef cookieStore, WKHTTPCookieAcceptPolicy policy, void* context, WKHTTPCookieStoreSetHTTPCookieAcceptPolicyFunction callback)
{
    protect(WebKit::toImpl(cookieStore))->setHTTPCookieAcceptPolicy(WebKit::toHTTPCookieAcceptPolicy(policy), [context, callback] {
        if (callback)
            callback(context);
    });
}

static WebCore::Cookie toCookie(const WKHTTPCookie& cookie)
{
    WebCore::Cookie result;
    result.name = String::fromUTF8(cookie.name);
    result.value = String::fromUTF8(cookie.value);
    result.domain = String::fromUTF8(cookie.domain);
    result.path = String::fromUTF8(cookie.path);
    result.session = cookie.session;
    if (!cookie.session)
        result.expires = cookie.expires;
    result.secure = cookie.secure;
    result.httpOnly = cookie.httpOnly;
    switch (cookie.sameSite) {
    case kWKHTTPCookieSameSiteLax:
        result.sameSite = WebCore::Cookie::SameSitePolicy::Lax;
        break;
    case kWKHTTPCookieSameSiteStrict:
        result.sameSite = WebCore::Cookie::SameSitePolicy::Strict;
        break;
    default:
        result.sameSite = WebCore::Cookie::SameSitePolicy::None;
        break;
    }
    return result;
}

static WKHTTPCookieSameSitePolicy toAPI(WebCore::Cookie::SameSitePolicy policy)
{
    switch (policy) {
    case WebCore::Cookie::SameSitePolicy::Lax:
        return kWKHTTPCookieSameSiteLax;
    case WebCore::Cookie::SameSitePolicy::Strict:
        return kWKHTTPCookieSameSiteStrict;
    case WebCore::Cookie::SameSitePolicy::None:
        break;
    }
    return kWKHTTPCookieSameSiteNone;
}

void WKHTTPCookieStoreGetAllCookies(WKHTTPCookieStoreRef cookieStore, void* context, WKHTTPCookieStoreGetCookiesFunction callback)
{
    protect(WebKit::toImpl(cookieStore))->cookies([context, callback](Vector<WebCore::Cookie>&& cookies) {
        if (!callback)
            return;
        Vector<std::array<CString, 4>> strings;
        strings.reserveInitialCapacity(cookies.size());
        Vector<WKHTTPCookie> result;
        result.reserveInitialCapacity(cookies.size());
        for (auto& cookie : cookies) {
            strings.append({ cookie.name.utf8(), cookie.value.utf8(), cookie.domain.utf8(), cookie.path.utf8() });
            auto& text = strings.last();
            result.append({
                text[0].data(),
                text[1].data(),
                text[2].data(),
                text[3].data(),
                cookie.expires.value_or(0),
                cookie.session || !cookie.expires,
                cookie.secure,
                cookie.httpOnly,
                toAPI(cookie.sameSite),
            });
        }
        callback(result.span().data(), result.size(), context);
    });
}

void WKHTTPCookieStoreSetCookie(WKHTTPCookieStoreRef cookieStore, const WKHTTPCookie* cookie, void* context, WKHTTPCookieStoreChangeFunction callback)
{
    protect(WebKit::toImpl(cookieStore))->setCookies({ toCookie(*cookie) }, [context, callback] {
        if (callback)
            callback(context);
    });
}

void WKHTTPCookieStoreDeleteCookie(WKHTTPCookieStoreRef cookieStore, const WKHTTPCookie* cookie, void* context, WKHTTPCookieStoreChangeFunction callback)
{
    protect(WebKit::toImpl(cookieStore))->deleteCookie(toCookie(*cookie), [context, callback] {
        if (callback)
            callback(context);
    });
}
