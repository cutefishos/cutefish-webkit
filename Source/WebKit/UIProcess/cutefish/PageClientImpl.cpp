/*
 * Copyright (C) 2021 Sony Interactive Entertainment Inc.
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
#include "PageClientImpl.h"

#include "DrawingAreaProxyCoordinatedGraphics.h"
#include "CutefishWebView.h"
#include "WebColorPicker.h"
#include "WebContextMenuProxyCutefish.h"
#include "WebDataListSuggestionsDropdown.h"
#include "WebDateTimePicker.h"
#include "ViewSnapshotStore.h"
#include "WebPageProxy.h"
#include "WebPopupMenuProxyCutefish.h"
#include <WebCore/DOMPasteAccess.h>
#include <WebCore/NotImplemented.h>
#include <wtf/TZoneMallocInlines.h>

namespace WebKit {

WTF_MAKE_TZONE_ALLOCATED_IMPL(PageClientImpl);

PageClientImpl::PageClientImpl(CutefishWebView& view)
    : m_view(view)
{
}

// PageClient's pure virtual functions
Ref<DrawingAreaProxy> PageClientImpl::createDrawingAreaProxy(WebProcessProxy& webProcessProxy)
{
    return DrawingAreaProxyCoordinatedGraphics::create(m_view.page(), webProcessProxy);
}

void PageClientImpl::setViewNeedsDisplay(const WebCore::Region&)
{
}

void PageClientImpl::requestScroll(const WebCore::FloatPoint& scrollPosition, const WebCore::IntPoint& scrollOrigin, WebCore::ScrollIsAnimated, WebCore::InterruptScrollAnimation)
{
}

WebCore::FloatPoint PageClientImpl::viewScrollPosition()
{
    return WebCore::FloatPoint { };
}

WebCore::IntSize PageClientImpl::viewSize()
{
    return m_view.size();
}

bool PageClientImpl::isViewWindowActive()
{
    return m_view.activityState().contains(WebCore::ActivityState::WindowIsActive);
}

bool PageClientImpl::isViewFocused()
{
    return m_view.activityState().contains(WebCore::ActivityState::IsFocused);
}

bool PageClientImpl::isActiveViewVisible()
{
    return m_view.activityState().contains(WebCore::ActivityState::IsVisible);
}

bool PageClientImpl::isViewInWindow()
{
    return m_view.activityState().contains(WebCore::ActivityState::IsInWindow);
}

void PageClientImpl::processDidExit()
{
}

void PageClientImpl::didRelaunchProcess()
{
}

void PageClientImpl::pageClosed()
{
}

void PageClientImpl::preferencesDidChange()
{
}

void PageClientImpl::toolTipChanged(const String&, const String& newToolTip)
{
    m_view.toolTipChanged(newToolTip);
}

void PageClientImpl::didCommitLoadForMainFrame(const String&, bool)
{
}

void PageClientImpl::didChangeContentSize(const WebCore::IntSize& size)
{
    m_view.contentsSizeChanged(size);
}

void PageClientImpl::selectionDidChange()
{
    m_view.selectionDidChange();
}

#if ENABLE(DRAG_SUPPORT)
void PageClientImpl::startDrag(WebCore::SelectionData&&, OptionSet<WebCore::DragOperation>, RefPtr<WebCore::ShareableBitmap>&&, WebCore::IntPoint&&)
{
    m_view.page().dragCancelled();
}
#endif

void PageClientImpl::setInputMethodState(std::optional<InputMethodState>&& state)
{
    m_view.setInputMethodEnabled(state.has_value());
}

RefPtr<ViewSnapshot> PageClientImpl::takeViewSnapshot(std::optional<WebCore::IntRect>&&)
{
    notImplemented();
    return nullptr;
}

void PageClientImpl::setCursor(const WebCore::Cursor& cursor)
{
    m_view.setCursor(cursor);
}

void PageClientImpl::setCursorHiddenUntilMouseMoves(bool)
{
}

void PageClientImpl::registerEditCommand(Ref<WebEditCommandProxy>&&, UndoOrRedo)
{
}

void PageClientImpl::clearAllEditCommands()
{
}

bool PageClientImpl::canUndoRedo(UndoOrRedo)
{
    return false;
}

void PageClientImpl::executeUndoRedo(UndoOrRedo)
{
}

void PageClientImpl::wheelEventWasNotHandledByWebCore(const NativeWebWheelEvent& event)
{
    m_view.wheelEventNotHandled(event);
}

WebCore::FloatRect PageClientImpl::convertToDeviceSpace(const WebCore::FloatRect& rect)
{
    return rect;
}

WebCore::FloatRect PageClientImpl::convertToUserSpace(const WebCore::FloatRect& rect)
{
    return rect;
}

WebCore::IntPoint PageClientImpl::screenToRootView(const WebCore::IntPoint& point)
{
    return point;
}

WebCore::IntPoint PageClientImpl::rootViewToScreen(const WebCore::IntPoint& point)
{
    return point;
}

WebCore::IntRect PageClientImpl::rootViewToScreen(const WebCore::IntRect& rect)
{
    return rect;
}

WebCore::IntPoint PageClientImpl::accessibilityScreenToRootView(const WebCore::IntPoint& point)
{
    return screenToRootView(point);
}

WebCore::IntRect PageClientImpl::rootViewToAccessibilityScreen(const WebCore::IntRect& rect)
{
    return rootViewToScreen(rect);    
}

void PageClientImpl::doneWithKeyEvent(const NativeWebKeyboardEvent&, bool)
{
}

#if ENABLE(TOUCH_EVENTS)
void PageClientImpl::doneWithTouchEvent(const WebTouchEvent&, bool)
{
}
#endif // ENABLE(TOUCH_EVENTS)

RefPtr<WebPopupMenuProxy> PageClientImpl::createPopupMenuProxy(WebPageProxy& page)
{
    return WebPopupMenuProxyCutefish::create(m_view, page.popupMenuClient());
}

#if ENABLE(CONTEXT_MENUS)
Ref<WebContextMenuProxy> PageClientImpl::createContextMenuProxy(WebPageProxy& page, FrameInfoData&& frameInfo, ContextMenuContextData&& context, const UserData& userData)
{
    return WebContextMenuProxyCutefish::create(page, m_view, WTF::move(frameInfo), WTF::move(context), userData);
}
#endif

RefPtr<WebColorPicker> PageClientImpl::createColorPicker(WebPageProxy&, const WebCore::Color&, const WebCore::IntRect&, ColorControlSupportsAlpha, Vector<WebCore::Color>&&, std::optional<WebCore::FrameIdentifier>)
{
    return nullptr;
}

RefPtr<WebDataListSuggestionsDropdown> PageClientImpl::createDataListSuggestionsDropdown(WebPageProxy&)
{
    return nullptr;
}

RefPtr<WebDateTimePicker> PageClientImpl::createDateTimePicker(WebPageProxy&)
{
    return nullptr;
}

void PageClientImpl::enterAcceleratedCompositingMode(const LayerTreeContext& context)
{
    m_view.updateAcceleratedSurface(context.contextID);
}

void PageClientImpl::exitAcceleratedCompositingMode()
{
    m_view.updateAcceleratedSurface(0);
}

void PageClientImpl::updateAcceleratedCompositingMode(const LayerTreeContext& context)
{
    m_view.updateAcceleratedSurface(context.contextID);
}

#if ENABLE(FULLSCREEN_API)
WebFullScreenManagerProxyClient& PageClientImpl::fullScreenManagerProxyClient()
{
    return *(WebFullScreenManagerProxyClient*)this;
}

void PageClientImpl::setFullScreenClientForTesting(std::unique_ptr<WebFullScreenManagerProxyClient>&&)
{
}

void PageClientImpl::closeFullScreenManager()
{
    m_view.closeFullScreenManager();
}

bool PageClientImpl::effectiveAppearanceIsDark() const
{
    return m_view.darkAppearance();
}

bool PageClientImpl::isFullScreen()
{
    return m_view.isFullScreen();
}

void PageClientImpl::enterFullScreen(WebCore::FloatSize, CompletionHandler<void(bool)>&& completionHandler)
{
    m_view.enterFullScreen(WTF::move(completionHandler));
}

void PageClientImpl::exitFullScreen(CompletionHandler<void()>&& completionHandler)
{
    m_view.exitFullScreen(WTF::move(completionHandler));
}

void PageClientImpl::beganEnterFullScreen(const WebCore::IntRect&, const WebCore::IntRect&, CompletionHandler<void(bool)>&& completionHandler)
{
    completionHandler(true);
}

void PageClientImpl::beganExitFullScreen(const WebCore::IntRect&, const WebCore::IntRect&, CompletionHandler<void()>&& completionHandler)
{
    completionHandler();
}
#endif

// Custom representations.
void PageClientImpl::didFinishLoadingDataForCustomContentProvider(const String& suggestedFilename, std::span<const uint8_t>)
{
}

void PageClientImpl::navigationGestureDidBegin()
{
}

void PageClientImpl::navigationGestureWillEnd(bool willNavigate, WebBackForwardListItem&)
{
}

void PageClientImpl::navigationGestureDidEnd(bool willNavigate, WebBackForwardListItem&)
{
}

void PageClientImpl::navigationGestureDidEnd()
{
}

void PageClientImpl::willRecordNavigationSnapshot(WebBackForwardListItem&)
{
}

void PageClientImpl::didRemoveNavigationGestureSnapshot()
{
}

void PageClientImpl::didFirstVisuallyNonEmptyLayoutForMainFrame()
{
}

void PageClientImpl::didFinishNavigation(API::Navigation*)
{
}

void PageClientImpl::didFailNavigation(API::Navigation*)
{
}

void PageClientImpl::didSameDocumentNavigationForMainFrame(SameDocumentNavigationType)
{
}

void PageClientImpl::didChangeBackgroundColor()
{
}

void PageClientImpl::underPageBackgroundColorDidChange()
{
    m_view.underPageBackgroundColorChanged();
}

void PageClientImpl::isPlayingAudioWillChange()
{
}

void PageClientImpl::isPlayingAudioDidChange()
{
}

void PageClientImpl::refView()
{
}

void PageClientImpl::derefView()
{
}

void PageClientImpl::didRestoreScrollPosition()
{
}

WebCore::UserInterfaceLayoutDirection PageClientImpl::userInterfaceLayoutDirection()
{
    return WebCore::UserInterfaceLayoutDirection::LTR;
}

void PageClientImpl::requestDOMPasteAccess(WebCore::DOMPasteAccessCategory, WebCore::DOMPasteRequiresInteraction, WebCore::FrameIdentifier, const WebCore::IntRect&, const String&, CompletionHandler<void(WebCore::DOMPasteAccessResponse)>&& completionHandler)
{
    completionHandler(WebCore::DOMPasteAccessResponse::DeniedForGesture);
}

} // namespace WebKit
