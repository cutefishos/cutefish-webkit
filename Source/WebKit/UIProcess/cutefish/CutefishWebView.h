// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "APIClient.h"
#include "APIObject.h"
#include "WKEventCutefish.h"
#include "WKViewClient.h"
#include <WebCore/ActivityState.h>
#include <WebCore/IntSize.h>
#include <wtf/OptionSet.h>
#include <wtf/TZoneMalloc.h>
#include <wtf/unix/UnixFileDescriptor.h>

namespace API {
template<> struct ClientTraits<WKViewClientBase> {
    typedef std::tuple<WKViewClientV0> Versions;
};
}

namespace API {
class PageConfiguration;
}

namespace WebCore {
class Cursor;
}

namespace WebKit {

class AcceleratedBackingStore;
class NativeWebWheelEvent;
class PageClientImpl;
class WebContextMenuProxyCutefish;
class WebPopupMenuProxyCutefish;
struct WebPopupItem;
class WebPageProxy;

class CutefishWebView final : public API::ObjectImpl<API::Object::Type::View> {
    WTF_MAKE_TZONE_ALLOCATED(CutefishWebView);
public:
    static Ref<CutefishWebView> create(const API::PageConfiguration&, const WebCore::IntSize&);
    virtual ~CutefishWebView();

    void setClient(const WKViewClientBase*);
    const WKViewClientV0& client() const { return m_client.client(); }

    WebPageProxy& page() { return *m_page; }

    void setSize(const WebCore::IntSize&);
    const WebCore::IntSize& size() const { return m_size; }
    void setDeviceScaleFactor(double);
    void setDarkAppearance(bool);
    bool darkAppearance() const { return m_darkAppearance; }

    void setActivityState(WebCore::ActivityState, bool);
    OptionSet<WebCore::ActivityState> activityState() const { return m_activityState; }

    void sendMouseEvent(const WKMouseEvent&);
    void sendWheelEvent(const WKWheelEvent&);
    void sendKeyboardEvent(const WKKeyboardEvent&);
    void sendTouchEvent(const ::WKTouchEvent&);

    void frameDisplayed();
    void releaseBuffer(uint64_t bufferIdentifier, UnixFileDescriptor&& releaseFence);

    void popupMenuItemSelected(int32_t index);
    void contextMenuItemSelected(int32_t index);

    void setComposition(const String&, unsigned cursor);
    void confirmComposition(const String&);
    void cancelComposition();

    void requestExitFullScreen();
    bool isFullScreen() const { return m_isFullScreen; }

    // Called by PageClientImpl.
    void updateAcceleratedSurface(uint64_t surfaceID);
    void setCursor(const WebCore::Cursor&);
    void setInputMethodEnabled(bool);
    void selectionDidChange();
    bool showPopupMenu(WebPopupMenuProxyCutefish&, const WebCore::IntRect&, const Vector<WebPopupItem>&, int32_t selectedIndex);
    void hidePopupMenu(WebPopupMenuProxyCutefish&);
    void showContextMenu(WebContextMenuProxyCutefish&, const Vector<uint32_t>& depths, const WebCore::IntPoint&);
    void toolTipChanged(const String&);
    void wheelEventNotHandled(const NativeWebWheelEvent&);
    void contentsSizeChanged(const WebCore::IntSize&);
    void underPageBackgroundColorChanged();
    void enterFullScreen(CompletionHandler<void(bool)>&&);
    void exitFullScreen(CompletionHandler<void()>&&);
    void closeFullScreenManager();

private:
    CutefishWebView(const API::PageConfiguration&, const WebCore::IntSize&);

    class ViewClient : public API::Client<WKViewClientBase> {
    public:
        explicit ViewClient(const WKViewClientBase* client) { initialize(client); }
        const WKViewClientV0& client() const { return m_client; }
    };

    std::unique_ptr<PageClientImpl> m_pageClient;
    RefPtr<WebPageProxy> m_page;
    ViewClient m_client { nullptr };
    WebCore::IntSize m_size;
    OptionSet<WebCore::ActivityState> m_activityState;
    bool m_isFullScreen { false };
    bool m_darkAppearance { false };
    bool m_inputMethodEnabled { false };
    RefPtr<WebPopupMenuProxyCutefish> m_popupMenu;
    RefPtr<WebContextMenuProxyCutefish> m_contextMenu;
    Ref<AcceleratedBackingStore> m_backingStore;
};

} // namespace WebKit

SPECIALIZE_TYPE_TRAITS_BEGIN(WebKit::CutefishWebView)
static bool isType(const API::Object& object) { return object.type() == API::Object::Type::View; }
SPECIALIZE_TYPE_TRAITS_END()
