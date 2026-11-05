/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc. All rights reserved.
 * Copyright (C) 2026 Reion Wong <reionwong@gmail.com>
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
#include "WebPageProxy.h"

#include "DRMMainDevice.h"
#include "DrawingAreaMessages.h"
#include "DrawingAreaProxy.h"
#include "EditorState.h"
#include "InputMethodState.h"
#include "PageClientImpl.h"
#include "RendererBufferFormat.h"
#include "WebPageProxyInternals.h"
#include "WebPreferences.h"
#include "WebProcessProxy.h"
#include <WebCore/NotImplemented.h>
#include <WebCore/PlatformEvent.h>
#include <WebCore/SearchPopupMenu.h>
#include <WebCore/UserAgent.h>
#include <wtf/CallbackAggregator.h>

#if USE(GBM)
#include <drm_fourcc.h>
#endif

namespace WebKit {

void WebPageProxy::platformInitialize()
{
}

void WebPageProxy::bindAccessibilityTree(const String&)
{
    notImplemented();
}

void WebPageProxy::didUpdateEditorState(const EditorState&, const EditorState& newEditorState)
{
    if (!newEditorState.shouldIgnoreSelectionChanges) {
        if (RefPtr pageClient = this->pageClient())
            pageClient->selectionDidChange();
    }
}

void WebPageProxy::setInputMethodState(std::optional<InputMethodState>&& state)
{
    if (RefPtr pageClient = this->pageClient())
        static_cast<PageClientImpl&>(*pageClient).setInputMethodState(WTF::move(state));
}

#if USE(GBM)
Vector<RendererBufferFormat> WebPageProxy::preferredBufferFormats() const
{
    auto device = drmMainDevice();
    if (device.isNull())
        return { };

    RendererBufferFormat bufferFormat;
    bufferFormat.usage = RendererBufferFormat::Usage::Rendering;
    bufferFormat.drmDevice = WTF::move(device);
    for (uint32_t fourcc : { DRM_FORMAT_ARGB8888, DRM_FORMAT_XRGB8888, DRM_FORMAT_ABGR8888, DRM_FORMAT_XBGR8888 })
        bufferFormat.formats.append({ fourcc, { DRM_FORMAT_MOD_INVALID } });
    return { WTF::move(bufferFormat) };
}
#endif

OptionSet<WebCore::PlatformEvent::Modifier> WebPageProxy::currentStateOfModifierKeys()
{
    return { };
}

void WebPageProxy::callAfterNextPresentationUpdate(CompletionHandler<void()>&& callback)
{
    if (!hasRunningProcess() || !m_drawingArea) {
        callback();
        return;
    }

    Ref aggregator = CallbackAggregator::create(WTF::move(callback));
    auto drawingAreaIdentifier = m_drawingArea->identifier();
    forEachWebContentProcess([&] (auto& process, auto) {
        process.sendWithAsyncReply(Messages::DrawingArea::DispatchAfterEnsuringDrawing(), [aggregator] { }, drawingAreaIdentifier);
    });
}

String WebPageProxy::userAgentForURL(const URL& url)
{
    if (url.isNull() || !preferences().needsSiteSpecificQuirks())
        return this->userAgent();

    auto userAgent = WebCore::standardUserAgentForURL(url);
    return userAgent.isNull() ? this->userAgent() : userAgent;
}

String WebPageProxy::standardUserAgent(const String& applicationNameForUserAgent)
{
    return WebCore::standardUserAgent(applicationNameForUserAgent);
}

void WebPageProxy::saveRecentSearches(IPC::Connection&, const String&, const Vector<WebCore::RecentSearch>&)
{
    notImplemented();
}

void WebPageProxy::loadRecentSearches(IPC::Connection&, const String&, CompletionHandler<void(Vector<WebCore::RecentSearch>&&)>&& completionHandler)
{
    notImplemented();
    completionHandler({ });
}

void WebPageProxy::scheduleActivityStateUpdate()
{
    if (internals().activityStateChangeTimer.isActive())
        return;

    internals().activityStateChangeTimer.startOneShot(0_s);
}

} // namespace WebKit
