// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WebContextMenuProxyCutefish.h"

#if ENABLE(CONTEXT_MENUS)

#include "CutefishWebView.h"
#include "WebContextMenuItem.h"
#include "WebPageProxy.h"

namespace WebKit {

WebContextMenuProxyCutefish::WebContextMenuProxyCutefish(WebPageProxy& page, CutefishWebView& view, FrameInfoData&& frameInfo, ContextMenuContextData&& context, const UserData& userData)
    : WebContextMenuProxy(page, WTF::move(frameInfo), WTF::move(context), userData)
    , m_view(view)
{
}

static void flatten(const WebContextMenuItemData& item, uint32_t depth, Vector<WebContextMenuItemData>& items, Vector<uint32_t>& depths)
{
    items.append(item);
    depths.append(depth);
    for (auto& child : item.submenu())
        flatten(child, depth + 1, items, depths);
}

void WebContextMenuProxyCutefish::showContextMenuWithItems(Vector<Ref<WebContextMenuItem>>&& items)
{
    m_items.clear();
    m_depths.clear();
    for (auto& item : items)
        flatten(item->data(), 0, m_items, m_depths);
    if (m_items.isEmpty())
        return;
    m_view.showContextMenu(*this, m_depths, menuLocation());
}

void WebContextMenuProxyCutefish::itemSelected(int32_t index)
{
    RefPtr page = this->page();
    if (!page || index < 0 || static_cast<size_t>(index) >= m_items.size())
        return;
    page->contextMenuItemSelected(m_items[index], frameInfo());
}

} // namespace WebKit

#endif // ENABLE(CONTEXT_MENUS)
