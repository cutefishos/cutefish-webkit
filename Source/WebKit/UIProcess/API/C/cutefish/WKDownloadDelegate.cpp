// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKDownloadDelegate.h"

#include "APIData.h"
#include "APIDownloadClient.h"
#include "APIURLRequest.h"
#include "AuthenticationChallengeDisposition.h"
#include "AuthenticationChallengeProxy.h"
#include "AuthenticationDecisionListener.h"
#include "DownloadProxy.h"
#include "WKAPICast.h"
#include "WebPageProxy.h"
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>

struct OpaqueWKDownloadListener {
    CompletionHandler<void(WebKit::AllowOverwrite, String)> destination;
    CompletionHandler<void(WebCore::ResourceRequest&&)> redirect;
    WebCore::ResourceRequest request;
};

namespace WebKit {

class DownloadDelegateCutefish final : public API::DownloadClient {
public:
    static Ref<DownloadDelegateCutefish> create(const WKDownloadDelegateBase& delegate)
    {
        return adoptRef(*new DownloadDelegateCutefish(delegate));
    }

private:
    explicit DownloadDelegateCutefish(const WKDownloadDelegateBase& delegate)
        : m_delegate(reinterpret_cast<const WKDownloadDelegateV0&>(delegate))
    {
    }

    void didReceiveAuthenticationChallenge(DownloadProxy& download, AuthenticationChallengeProxy& challenge) final
    {
        if (!m_delegate.didReceiveAuthenticationChallenge) {
            challenge.listener().completeChallenge(AuthenticationChallengeDisposition::RejectProtectionSpaceAndContinue);
            return;
        }
        m_delegate.didReceiveAuthenticationChallenge(toAPI(download), toAPI(challenge), m_delegate.base.clientInfo);
    }

    void didReceiveData(DownloadProxy& download, uint64_t bytesWritten, uint64_t totalBytesWritten, uint64_t totalBytesExpectedToWrite) final
    {
        if (m_delegate.didWriteData)
            m_delegate.didWriteData(toAPI(download), bytesWritten, totalBytesWritten, totalBytesExpectedToWrite, m_delegate.base.clientInfo);
    }

    void decideDestinationWithSuggestedFilename(DownloadProxy& download, const WebCore::ResourceResponse& response, const String& suggestedFilename, CompletionHandler<void(AllowOverwrite, String)>&& completionHandler) final
    {
        if (!m_delegate.decideDestination) {
            completionHandler(AllowOverwrite::No, { });
            return;
        }
        auto* listener = new OpaqueWKDownloadListener;
        listener->destination = WTF::move(completionHandler);
        m_delegate.decideDestination(toAPI(download), toAPI(response), toAPI(suggestedFilename.impl()), listener, m_delegate.base.clientInfo);
    }

    void didFinish(DownloadProxy& download) final
    {
        if (m_delegate.didFinish)
            m_delegate.didFinish(toAPI(download), m_delegate.base.clientInfo);
    }

    void didFail(DownloadProxy& download, const WebCore::ResourceError& error, API::Data* resumeData) final
    {
        if (m_delegate.didFailWithError)
            m_delegate.didFailWithError(toAPI(download), toAPI(error), toAPI(resumeData), m_delegate.base.clientInfo);
    }

    void processDidCrash(DownloadProxy& download) final
    {
        didFail(download, WebCore::ResourceError { WebCore::errorDomainWebKitInternal, 0, download.request().url(), "Network process crashed during download"_s }, nullptr);
    }

    void willSendRequest(DownloadProxy& download, WebCore::ResourceRequest&& request, const WebCore::ResourceResponse& response, CompletionHandler<void(WebCore::ResourceRequest&&)>&& completionHandler) final
    {
        if (!m_delegate.willPerformHTTPRedirection) {
            completionHandler(WTF::move(request));
            return;
        }
        auto* listener = new OpaqueWKDownloadListener;
        listener->redirect = WTF::move(completionHandler);
        listener->request = request;
        m_delegate.willPerformHTTPRedirection(toAPI(download), toAPI(response), toAPI(API::URLRequest::create(WTF::move(request)).ptr()), listener, m_delegate.base.clientInfo);
    }

    const WKDownloadDelegateV0 m_delegate;
};

} // namespace WebKit

using namespace WebKit;

void WKDownloadSetDelegate(WKDownloadRef download, const WKDownloadDelegateBase* delegate)
{
    if (!delegate) {
        protect(toImpl(download))->setClient(adoptRef(*new API::DownloadClient));
        return;
    }
    protect(toImpl(download))->setClient(DownloadDelegateCutefish::create(*delegate));
}

void WKDownloadListenerChooseDestination(WKDownloadListenerRef listener, const char* path, bool allowOverwrite)
{
    if (listener->destination)
        listener->destination(allowOverwrite ? AllowOverwrite::Yes : AllowOverwrite::No, path ? String::fromUTF8(path) : String());
    delete listener;
}

void WKDownloadListenerAllowRedirect(WKDownloadListenerRef listener, bool allow)
{
    if (listener->redirect)
        listener->redirect(allow ? WTF::move(listener->request) : WebCore::ResourceRequest());
    delete listener;
}

void WKPageDownloadRequest(WKPageRef page, WKURLRequestRef request, void* context, WKPageDownloadStartedCallback callback)
{
    WebCore::ResourceRequest resourceRequest = toImpl(request)->resourceRequest();
    protect(toImpl(page))->downloadRequest(WTF::move(resourceRequest), [context, callback](DownloadProxy* download) {
        if (callback)
            callback(toAPI(download), context);
    });
}
