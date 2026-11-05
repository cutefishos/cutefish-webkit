// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "CutefishWebView.h"

#include "APIPageConfiguration.h"
#include "AcceleratedBackingStore.h"
#include "DrawingAreaProxy.h"
#include "EditorState.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "NativeWebTouchEvent.h"
#include "NativeWebWheelEvent.h"
#include "PageClientImpl.h"
#include "WKAPICast.h"
#include "WebContextMenuProxyCutefish.h"
#include "WebFullScreenManagerProxy.h"
#include "WebPageProxy.h"
#include "WebPopupItem.h"
#include "WebPopupMenuProxyCutefish.h"
#include "WebProcessPool.h"
#include "WebURLSchemeHandler.h"
#include <WebCore/Color.h>
#include <WebCore/CompositionUnderline.h>
#include <WebCore/Cursor.h>
#include <WebCore/PlatformKeyboardEvent.h>
#include <wtf/TZoneMallocInlines.h>
#include <wtf/text/CString.h>
#include <xkbcommon/xkbcommon.h>

namespace WebKit {

WTF_MAKE_TZONE_ALLOCATED_IMPL(CutefishWebView);

Ref<CutefishWebView> CutefishWebView::create(const API::PageConfiguration& configuration, const WebCore::IntSize& size)
{
    return adoptRef(*new CutefishWebView(configuration, size));
}

CutefishWebView::CutefishWebView(const API::PageConfiguration& baseConfiguration, const WebCore::IntSize& size)
    : m_pageClient(makeUniqueWithoutRefCountedCheck<PageClientImpl>(*this))
    , m_size(size)
    , m_activityState { WebCore::ActivityState::WindowIsActive, WebCore::ActivityState::IsFocused, WebCore::ActivityState::IsVisible, WebCore::ActivityState::IsInWindow }
    , m_backingStore(AcceleratedBackingStore::create(*this))
{
    auto configuration = baseConfiguration.copy();
    Ref pool = configuration->processPool();
    auto schemeHandlers = configuration->urlSchemeHandlers();
    m_page = pool->createWebPage(*m_pageClient, WTF::move(configuration));
    for (auto& [scheme, handler] : schemeHandlers)
        m_page->setURLSchemeHandlerForScheme(handler.copyRef(), scheme);

    m_page->setBackgroundColor(WebCore::Color::white);

    // The display link only runs for a non-zero display.
    static constexpr WebCore::PlatformDisplayID displayID = 1;
    m_page->windowScreenDidChange(displayID);

    auto& pageConfiguration = m_page->configuration();
    m_page->initializeWebPage(pageConfiguration.openedSite(), pageConfiguration.initialSandboxFlags(), pageConfiguration.initialReferrerPolicy());
}

CutefishWebView::~CutefishWebView()
{
    m_backingStore->updateSurfaceID(0);
    m_page->close();
}

void CutefishWebView::setClient(const WKViewClientBase* client)
{
    m_client.initialize(client);
}

void CutefishWebView::setSize(const WebCore::IntSize& size)
{
    if (m_size == size)
        return;

    m_size = size;
    if (RefPtr drawingArea = m_page->drawingArea())
        drawingArea->setSize(size);
}

void CutefishWebView::setDeviceScaleFactor(double scaleFactor)
{
    m_page->setIntrinsicDeviceScaleFactor(scaleFactor);
}

void CutefishWebView::setDarkAppearance(bool dark)
{
    if (dark == m_darkAppearance)
        return;
    m_darkAppearance = dark;
    m_page->effectiveAppearanceDidChange();
}

void CutefishWebView::setActivityState(WebCore::ActivityState flag, bool set)
{
    auto state = m_activityState;
    if (set)
        state.add(flag);
    else
        state.remove(flag);

    auto changedFlags = m_activityState ^ state;
    m_activityState = state;
    if (changedFlags)
        m_page->activityStateDidChange(changedFlags);
}

static OptionSet<WebEventModifier> modifiersFromAPI(WKEventModifiers modifiers)
{
    OptionSet<WebEventModifier> result;
    if (modifiers & kWKEventModifiersShiftKey)
        result.add(WebEventModifier::ShiftKey);
    if (modifiers & kWKEventModifiersControlKey)
        result.add(WebEventModifier::ControlKey);
    if (modifiers & kWKEventModifiersAltKey)
        result.add(WebEventModifier::AltKey);
    if (modifiers & kWKEventModifiersMetaKey)
        result.add(WebEventModifier::MetaKey);
    if (modifiers & kWKEventModifiersCapsLockKey)
        result.add(WebEventModifier::CapsLockKey);
    return result;
}

static MonotonicTime timestampFromAPI(double timestamp)
{
    return timestamp > 0 ? MonotonicTime::fromRawSeconds(timestamp) : MonotonicTime::now();
}

static WebMouseEventButton mouseButtonFromAPI(WKEventMouseButton button)
{
    switch (button) {
    case kWKEventMouseButtonLeftButton:
        return WebMouseEventButton::Left;
    case kWKEventMouseButtonMiddleButton:
        return WebMouseEventButton::Middle;
    case kWKEventMouseButtonRightButton:
        return WebMouseEventButton::Right;
    case kWKEventMouseButtonBackButton:
        return WebMouseEventButton::Back;
    case kWKEventMouseButtonForwardButton:
        return WebMouseEventButton::Forward;
    default:
        return WebMouseEventButton::None;
    }
}

void CutefishWebView::sendMouseEvent(const WKMouseEvent& event)
{
    WebEventType type;
    switch (event.type) {
    case kWKEventMouseDown:
        type = WebEventType::MouseDown;
        break;
    case kWKEventMouseUp:
        type = WebEventType::MouseUp;
        break;
    case kWKEventMouseMove:
        type = WebEventType::MouseMove;
        break;
    default:
        return;
    }

    WebCore::DoublePoint position { event.position.x, event.position.y };
    WebCore::DoublePoint globalPosition { event.globalPosition.x, event.globalPosition.y };
    WebMouseEvent mouseEvent({ type, modifiersFromAPI(event.modifiers), timestampFromAPI(event.timestamp) },
        mouseButtonFromAPI(event.button), event.buttons, position, globalPosition, 0, 0, 0, event.clickCount, 0, WebEventInputSource::UserDriven);
    m_page->handleMouseEvent(NativeWebMouseEvent(WTF::move(mouseEvent)));
}

static WebWheelEvent::Phase wheelPhaseFromAPI(WKWheelEventPhase phase)
{
    switch (phase) {
    case kWKWheelEventPhaseBegan:
        return WebWheelEvent::Phase::Began;
    case kWKWheelEventPhaseChanged:
        return WebWheelEvent::Phase::Changed;
    case kWKWheelEventPhaseEnded:
        return WebWheelEvent::Phase::Ended;
    case kWKWheelEventPhaseCancelled:
        return WebWheelEvent::Phase::Cancelled;
    default:
        return WebWheelEvent::Phase::None;
    }
}

void CutefishWebView::sendWheelEvent(const WKWheelEvent& event)
{
    WebCore::IntPoint position { static_cast<int>(event.position.x), static_cast<int>(event.position.y) };
    WebCore::IntPoint globalPosition { static_cast<int>(event.globalPosition.x), static_cast<int>(event.globalPosition.y) };

    // WebCore scrolls towards the start of the content for positive deltas.
    WebCore::FloatSize delta { static_cast<float>(-event.delta.width), static_cast<float>(-event.delta.height) };
    WebCore::FloatSize wheelTicks { static_cast<float>(-event.wheelTicks.width), static_cast<float>(-event.wheelTicks.height) };

    WebWheelEvent wheelEvent({ WebEventType::Wheel, modifiersFromAPI(event.modifiers), timestampFromAPI(event.timestamp) },
        position, globalPosition, delta, wheelTicks, WebWheelEvent::Granularity::ScrollByPixelWheelEvent,
        wheelPhaseFromAPI(event.phase), wheelPhaseFromAPI(event.momentumPhase), event.hasPreciseDeltas);
    m_page->handleNativeWheelEvent(NativeWebWheelEvent(WTF::move(wheelEvent)));
}

void CutefishWebView::sendKeyboardEvent(const WKKeyboardEvent& event)
{
    WebEventType type;
    switch (event.type) {
    case kWKEventKeyDown:
        type = WebEventType::KeyDown;
        break;
    case kWKEventKeyUp:
        type = WebEventType::KeyUp;
        break;
    default:
        return;
    }

    String text = event.text ? String::fromUTF8(event.text) : WebCore::PlatformKeyboardEvent::singleCharacterString(event.keysym);
    WebKeyboardEvent keyboardEvent({ type, modifiersFromAPI(event.modifiers), timestampFromAPI(event.timestamp) },
        text,
        WebCore::PlatformKeyboardEvent::keyValueForKeysym(event.keysym),
        WebCore::PlatformKeyboardEvent::keyCodeForHardwareKeyCode(event.keycode),
        WebCore::PlatformKeyboardEvent::keyIdentifierForKeysym(event.keysym),
        WebCore::PlatformKeyboardEvent::windowsKeyCodeForKeysym(event.keysym),
        event.keysym, false, std::nullopt, std::nullopt, event.isAutoRepeat,
        event.isKeypad || (event.keysym >= XKB_KEY_KP_Space && event.keysym <= XKB_KEY_KP_9));
    m_page->handleKeyboardEvent(NativeWebKeyboardEvent(WTF::move(keyboardEvent)));
}

void CutefishWebView::sendTouchEvent(const ::WKTouchEvent& event)
{
#if ENABLE(TOUCH_EVENTS)
    WebEventType type;
    switch (event.type) {
    case kWKEventTouchStart:
        type = WebEventType::TouchStart;
        break;
    case kWKEventTouchMove:
        type = WebEventType::TouchMove;
        break;
    case kWKEventTouchEnd:
        type = WebEventType::TouchEnd;
        break;
    case kWKEventTouchCancel:
        type = WebEventType::TouchCancel;
        break;
    default:
        return;
    }

    Vector<WebPlatformTouchPoint> points;
    points.reserveInitialCapacity(event.pointCount);
    for (auto& point : unsafeMakeSpan(event.points, event.pointCount)) {
        WebPlatformTouchPoint::State state;
        switch (point.state) {
        case kWKTouchPointPressed:
            state = WebPlatformTouchPoint::State::Pressed;
            break;
        case kWKTouchPointMoved:
            state = WebPlatformTouchPoint::State::Moved;
            break;
        case kWKTouchPointReleased:
            state = WebPlatformTouchPoint::State::Released;
            break;
        case kWKTouchPointCancelled:
            state = WebPlatformTouchPoint::State::Cancelled;
            break;
        default:
            state = WebPlatformTouchPoint::State::Stationary;
            break;
        }
        points.append(WebPlatformTouchPoint(point.id, state, { point.globalPosition.x, point.globalPosition.y }, { point.position.x, point.position.y }));
    }

    WebTouchEvent touchEvent({ type, modifiersFromAPI(event.modifiers), timestampFromAPI(event.timestamp) }, WTF::move(points), { }, { });
    m_page->handleTouchEvent(nullptr, NativeWebTouchEvent(WTF::move(touchEvent)));
#else
    UNUSED_PARAM(event);
#endif
}

void CutefishWebView::frameDisplayed()
{
    m_backingStore->frameDisplayed();
}

void CutefishWebView::releaseBuffer(uint64_t bufferIdentifier, UnixFileDescriptor&& releaseFence)
{
    m_backingStore->releaseBuffer(bufferIdentifier, WTF::move(releaseFence));
}

void CutefishWebView::updateAcceleratedSurface(uint64_t surfaceID)
{
    m_backingStore->updateSurfaceID(surfaceID);
}

static ASCIILiteral cssCursorName(WebCore::Cursor::Type type)
{
    using Type = WebCore::Cursor::Type;
    switch (type) {
    case Type::Cross:
        return "crosshair"_s;
    case Type::Hand:
        return "pointer"_s;
    case Type::IBeam:
        return "text"_s;
    case Type::Wait:
        return "wait"_s;
    case Type::Help:
        return "help"_s;
    case Type::EastResize:
        return "e-resize"_s;
    case Type::NorthResize:
        return "n-resize"_s;
    case Type::NorthEastResize:
        return "ne-resize"_s;
    case Type::NorthWestResize:
        return "nw-resize"_s;
    case Type::SouthResize:
        return "s-resize"_s;
    case Type::SouthEastResize:
        return "se-resize"_s;
    case Type::SouthWestResize:
        return "sw-resize"_s;
    case Type::WestResize:
        return "w-resize"_s;
    case Type::NorthSouthResize:
        return "ns-resize"_s;
    case Type::EastWestResize:
        return "ew-resize"_s;
    case Type::NorthEastSouthWestResize:
        return "nesw-resize"_s;
    case Type::NorthWestSouthEastResize:
        return "nwse-resize"_s;
    case Type::ColumnResize:
        return "col-resize"_s;
    case Type::RowResize:
        return "row-resize"_s;
    case Type::MiddlePanning:
    case Type::EastPanning:
    case Type::NorthPanning:
    case Type::NorthEastPanning:
    case Type::NorthWestPanning:
    case Type::SouthPanning:
    case Type::SouthEastPanning:
    case Type::SouthWestPanning:
    case Type::WestPanning:
    case Type::Move:
        return "move"_s;
    case Type::VerticalText:
        return "vertical-text"_s;
    case Type::Cell:
        return "cell"_s;
    case Type::ContextMenu:
        return "context-menu"_s;
    case Type::Alias:
        return "alias"_s;
    case Type::Progress:
        return "progress"_s;
    case Type::NoDrop:
        return "no-drop"_s;
    case Type::Copy:
        return "copy"_s;
    case Type::None:
        return "none"_s;
    case Type::NotAllowed:
        return "not-allowed"_s;
    case Type::ZoomIn:
        return "zoom-in"_s;
    case Type::ZoomOut:
        return "zoom-out"_s;
    case Type::Grab:
        return "grab"_s;
    case Type::Grabbing:
        return "grabbing"_s;
    case Type::Invalid:
    case Type::Pointer:
    case Type::Custom:
        break;
    }
    return "default"_s;
}

void CutefishWebView::setCursor(const WebCore::Cursor& cursor)
{
    auto& client = this->client();
    if (client.setCursor)
        client.setCursor(toAPI(this), cssCursorName(cursor.type()).characters(), client.base.clientInfo);
}

void CutefishWebView::setComposition(const String& text, unsigned cursor)
{
    Vector<WebCore::CompositionUnderline> underlines {
        WebCore::CompositionUnderline(0, text.length(), WebCore::CompositionUnderlineColor::TextColor, WebCore::Color::black, false)
    };
    WebKeyboardEvent event({ WebEventType::KeyDown, { }, MonotonicTime::now() }, text, "Unidentified"_s, "Unidentified"_s, "U+0000"_s,
        0, 0, true, WTF::move(underlines), EditingRange(std::min<unsigned>(cursor, text.length()), 0), false, false);
    m_page->handleKeyboardEvent(NativeWebKeyboardEvent(WTF::move(event)));
}

void CutefishWebView::confirmComposition(const String& text)
{
    WebKeyboardEvent event({ WebEventType::KeyDown, { }, MonotonicTime::now() }, text, "Unidentified"_s, "Unidentified"_s, "U+0000"_s,
        0, 0, true, std::nullopt, std::nullopt, false, false);
    m_page->handleKeyboardEvent(NativeWebKeyboardEvent(WTF::move(event)));
}

void CutefishWebView::cancelComposition()
{
    m_page->cancelComposition(String());
}

void CutefishWebView::setInputMethodEnabled(bool enabled)
{
    if (m_inputMethodEnabled == enabled)
        return;
    m_inputMethodEnabled = enabled;
    selectionDidChange();
}

void CutefishWebView::selectionDidChange()
{
    auto& client = this->client();
    if (!client.inputMethodStateChanged)
        return;
    WebCore::IntRect caret;
    auto& state = m_page->editorState();
    if (state.hasVisualData())
        caret = state.visualData->caretRectAtStart;
    client.inputMethodStateChanged(toAPI(this), m_inputMethodEnabled, toAPI(caret), client.base.clientInfo);
}

bool CutefishWebView::showPopupMenu(WebPopupMenuProxyCutefish& menu, const WebCore::IntRect& rect, const Vector<WebPopupItem>& items, int32_t selectedIndex)
{
    auto& client = this->client();
    if (!client.showPopupMenu)
        return false;
    m_popupMenu = &menu;
    Vector<CString> titles;
    titles.reserveInitialCapacity(items.size());
    Vector<WKViewMenuItem> menuItems;
    menuItems.reserveInitialCapacity(items.size());
    for (auto& item : items) {
        titles.append(item.m_text.utf8());
        bool separator = item.m_type == WebPopupItem::Type::Separator;
        menuItems.append({ titles.last().data(), separator ? kWKViewMenuItemSeparator : kWKViewMenuItemAction, 0, 0, item.m_isEnabled, item.m_isSelected, item.m_isLabel });
    }
    client.showPopupMenu(toAPI(this), menuItems.span().data(), menuItems.size(), selectedIndex, toAPI(rect), client.base.clientInfo);
    return true;
}

void CutefishWebView::hidePopupMenu(WebPopupMenuProxyCutefish& menu)
{
    if (m_popupMenu != &menu)
        return;
    m_popupMenu = nullptr;
    auto& client = this->client();
    if (client.hidePopupMenu)
        client.hidePopupMenu(toAPI(this), client.base.clientInfo);
}

void CutefishWebView::popupMenuItemSelected(int32_t index)
{
    if (RefPtr menu = std::exchange(m_popupMenu, nullptr))
        menu->itemSelected(index);
}

static WKViewMenuItemType menuItemType(WebCore::ContextMenuItemType type)
{
    switch (type) {
    case WebCore::ContextMenuItemType::Action:
        return kWKViewMenuItemAction;
    case WebCore::ContextMenuItemType::CheckableAction:
        return kWKViewMenuItemCheckable;
    case WebCore::ContextMenuItemType::Separator:
        return kWKViewMenuItemSeparator;
    case WebCore::ContextMenuItemType::Submenu:
        return kWKViewMenuItemSubmenu;
    }
    return kWKViewMenuItemAction;
}

void CutefishWebView::showContextMenu(WebContextMenuProxyCutefish& menu, const Vector<uint32_t>& depths, const WebCore::IntPoint& location)
{
    auto& client = this->client();
    if (!client.showContextMenu)
        return;
    m_contextMenu = &menu;
    auto& items = menu.items();
    Vector<CString> titles;
    titles.reserveInitialCapacity(items.size());
    Vector<WKViewMenuItem> menuItems;
    menuItems.reserveInitialCapacity(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        auto& item = items[i];
        titles.append(item.title().utf8());
        menuItems.append({ titles.last().data(), menuItemType(item.type()), toAPI(item.action()), depths[i], item.enabled(), item.checked(), false });
    }
    client.showContextMenu(toAPI(this), menuItems.span().data(), menuItems.size(), toAPI(location), client.base.clientInfo);
}

void CutefishWebView::contextMenuItemSelected(int32_t index)
{
    RefPtr menu = std::exchange(m_contextMenu, nullptr);
    if (menu && index >= 0)
        menu->itemSelected(index);
}

void CutefishWebView::toolTipChanged(const String& toolTip)
{
    auto& client = this->client();
    if (client.toolTipChanged)
        client.toolTipChanged(toAPI(this), toolTip.utf8().data(), client.base.clientInfo);
}

void CutefishWebView::wheelEventNotHandled(const NativeWebWheelEvent& event)
{
    auto& client = this->client();
    if (!client.wheelEventNotHandled)
        return;
    // Back to the client's direction: towards the end of the content.
    auto delta = event.delta();
    client.wheelEventNotHandled(toAPI(this), WKSizeMake(-delta.width(), -delta.height()), client.base.clientInfo);
}

void CutefishWebView::contentsSizeChanged(const WebCore::IntSize& size)
{
    auto& client = this->client();
    if (client.contentsSizeChanged)
        client.contentsSizeChanged(toAPI(this), WKSizeMake(size.width(), size.height()), client.base.clientInfo);
}

void CutefishWebView::underPageBackgroundColorChanged()
{
    auto& client = this->client();
    if (!client.underPageBackgroundColorChanged)
        return;
    auto [red, green, blue, alpha] = m_page->underPageBackgroundColor().toColorTypeLossy<WebCore::SRGBA<float>>().resolved();
    client.underPageBackgroundColorChanged(toAPI(this), red, green, blue, alpha, client.base.clientInfo);
}

void CutefishWebView::enterFullScreen(CompletionHandler<void(bool)>&& completionHandler)
{
    auto& client = this->client();
    if (m_isFullScreen || !client.enterFullScreen) {
        completionHandler(false);
        return;
    }

    m_isFullScreen = true;
    client.enterFullScreen(toAPI(this), client.base.clientInfo);
    completionHandler(true);
}

void CutefishWebView::exitFullScreen(CompletionHandler<void()>&& completionHandler)
{
    auto& client = this->client();
    if (m_isFullScreen && client.exitFullScreen)
        client.exitFullScreen(toAPI(this), client.base.clientInfo);
    m_isFullScreen = false;
    completionHandler();
}

void CutefishWebView::closeFullScreenManager()
{
    m_isFullScreen = false;
}

void CutefishWebView::requestExitFullScreen()
{
#if ENABLE(FULLSCREEN_API)
    if (m_isFullScreen) {
        if (RefPtr manager = m_page->fullScreenManager())
            manager->requestExitFullScreen();
    }
#endif
}

} // namespace WebKit
