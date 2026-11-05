// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#if ENABLE(CONTEXT_MENUS)

#include "WebContextMenuItemData.h"
#include "WebContextMenuProxy.h"

namespace WebKit {

class CutefishWebView;

class WebContextMenuProxyCutefish final : public WebContextMenuProxy {
public:
    static Ref<WebContextMenuProxyCutefish> create(WebPageProxy& page, CutefishWebView& view, FrameInfoData&& frameInfo, ContextMenuContextData&& context, const UserData& userData)
    {
        return adoptRef(*new WebContextMenuProxyCutefish(page, view, WTF::move(frameInfo), WTF::move(context), userData));
    }

    // Items in the order the client sees them, submenus flattened.
    const Vector<WebContextMenuItemData>& items() const LIFETIME_BOUND { return m_items; }
    void itemSelected(int32_t index);

private:
    WebContextMenuProxyCutefish(WebPageProxy&, CutefishWebView&, FrameInfoData&&, ContextMenuContextData&&, const UserData&);

    void showContextMenuWithItems(Vector<Ref<WebContextMenuItem>>&&) final;

    CutefishWebView& m_view;
    Vector<WebContextMenuItemData> m_items;
    Vector<uint32_t> m_depths;
};

} // namespace WebKit

#endif // ENABLE(CONTEXT_MENUS)
