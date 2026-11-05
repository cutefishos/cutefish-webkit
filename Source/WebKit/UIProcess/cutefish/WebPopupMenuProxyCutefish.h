// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "WebPopupMenuProxy.h"

namespace WebKit {

class CutefishWebView;

class WebPopupMenuProxyCutefish final : public WebPopupMenuProxy {
public:
    static Ref<WebPopupMenuProxyCutefish> create(CutefishWebView& view, WebPopupMenuProxy::Client& client)
    {
        return adoptRef(*new WebPopupMenuProxyCutefish(view, client));
    }

    void showPopupMenu(const WebCore::IntRect&, WebCore::TextDirection, double pageScaleFactor, const Vector<WebPopupItem>&, const PlatformPopupMenuData&, int32_t selectedIndex) final;
    void hidePopupMenu() final;
    void cancelTracking() final;

    void itemSelected(int32_t index);

private:
    WebPopupMenuProxyCutefish(CutefishWebView&, WebPopupMenuProxy::Client&);

    CutefishWebView& m_view;
};

} // namespace WebKit
