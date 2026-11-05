// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKURLSchemeHandler.h"

#include "APIPageConfiguration.h"
#include "APIURLRequest.h"
#include "WKAPICast.h"
#include "WebURLSchemeHandler.h"
#include "WebURLSchemeTask.h"
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceResponse.h>
#include <WebCore/SharedBuffer.h>

namespace WebKit {

WK_ADD_API_MAPPING(WKURLSchemeTaskRef, WebURLSchemeTask)

class URLSchemeHandlerCutefish final : public WebURLSchemeHandler {
public:
    static Ref<URLSchemeHandlerCutefish> create(const WKURLSchemeHandlerClientBase& client)
    {
        return adoptRef(*new URLSchemeHandlerCutefish(client));
    }

private:
    explicit URLSchemeHandlerCutefish(const WKURLSchemeHandlerClientBase& client)
        : m_client(reinterpret_cast<const WKURLSchemeHandlerClientV0&>(client))
    {
    }

    void platformStartTask(WebPageProxy&, WebURLSchemeTask& task) final
    {
        if (m_client.startTask)
            m_client.startTask(toAPI(&task), m_client.base.clientInfo);
    }

    void platformStopTask(WebPageProxy&, WebURLSchemeTask& task) final
    {
        if (m_client.stopTask)
            m_client.stopTask(toAPI(&task), m_client.base.clientInfo);
    }

    const WKURLSchemeHandlerClientV0 m_client;
};

} // namespace WebKit

SPECIALIZE_TYPE_TRAITS_BEGIN(WebKit::WebURLSchemeTask)
static bool isType(const API::Object& object) { return object.type() == API::Object::Type::URLSchemeTask; }
SPECIALIZE_TYPE_TRAITS_END()

using namespace WebKit;

void WKPageConfigurationSetURLSchemeHandler(WKPageConfigurationRef configuration, const char* scheme, const WKURLSchemeHandlerClientBase* client)
{
    if (!scheme || !client)
        return;
    toImpl(configuration)->setURLSchemeHandlerForURLScheme(URLSchemeHandlerCutefish::create(*client), String::fromUTF8(scheme));
}

WKURLRequestRef WKURLSchemeTaskCopyRequest(WKURLSchemeTaskRef task)
{
    return toAPILeakingRef(API::URLRequest::create(toImpl(task)->request()));
}

void WKURLSchemeTaskDidReceiveResponse(WKURLSchemeTaskRef task, const char* mimeType, int64_t expectedLength, const char* textEncodingName, int statusCode)
{
    Ref schemeTask = *toImpl(task);
    WebCore::ResourceResponse response(URL { schemeTask->request().url() }, String::fromUTF8(mimeType), expectedLength, String::fromUTF8(textEncodingName));
    if (statusCode)
        response.setHTTPStatusCode(statusCode);
    schemeTask->didReceiveResponse(WTF::move(response));
}

void WKURLSchemeTaskDidReceiveData(WKURLSchemeTaskRef task, const void* bytes, size_t length)
{
    Ref schemeTask = *toImpl(task);
    schemeTask->didReceiveData(WebCore::SharedBuffer::create(unsafeMakeSpan(static_cast<const uint8_t*>(bytes), length)));
}

void WKURLSchemeTaskDidFinish(WKURLSchemeTaskRef task)
{
    Ref schemeTask = *toImpl(task);
    schemeTask->didComplete({ });
}

void WKURLSchemeTaskDidFail(WKURLSchemeTaskRef task, const char* domain, int code, const char* localizedDescription)
{
    Ref schemeTask = *toImpl(task);
    URL url = schemeTask->request().url();
    schemeTask->didComplete(WebCore::ResourceError(String::fromUTF8(domain), code, url, String::fromUTF8(localizedDescription)));
}
