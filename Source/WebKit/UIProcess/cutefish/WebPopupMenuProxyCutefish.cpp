// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WebPopupMenuProxyCutefish.h"

#include "CutefishWebView.h"
#include "WebPopupItem.h"

namespace WebKit {

WebPopupMenuProxyCutefish::WebPopupMenuProxyCutefish(CutefishWebView& view, WebPopupMenuProxy::Client& client)
    : WebPopupMenuProxy(client)
    , m_view(view)
{
}

void WebPopupMenuProxyCutefish::showPopupMenu(const WebCore::IntRect& rect, WebCore::TextDirection, double, const Vector<WebPopupItem>& items, const PlatformPopupMenuData&, int32_t selectedIndex)
{
    if (m_view.showPopupMenu(*this, rect, items, selectedIndex))
        return;
    if (CheckedPtr client = this->client())
        client->failedToShowPopupMenu();
}

void WebPopupMenuProxyCutefish::hidePopupMenu()
{
    m_view.hidePopupMenu(*this);
}

void WebPopupMenuProxyCutefish::cancelTracking()
{
    hidePopupMenu();
}

void WebPopupMenuProxyCutefish::itemSelected(int32_t index)
{
    if (CheckedPtr client = this->client())
        client->valueChangedForPopupMenu(this, index);
}

} // namespace WebKit
