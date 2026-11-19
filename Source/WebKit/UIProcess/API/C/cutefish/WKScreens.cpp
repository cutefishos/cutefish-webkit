// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKScreens.h"

#include "ScreensCutefish.h"
#include "WebProcessMessages.h"
#include "WebProcessPool.h"
#include <wtf/NeverDestroyed.h>

namespace WebKit {

WebCore::ScreenProperties& cutefishScreenProperties()
{
    static NeverDestroyed<WebCore::ScreenProperties> properties;
    return properties;
}

} // namespace WebKit

void WKSetScreens(const WKScreenInfo* screens, size_t count, uint32_t primaryIdentifier)
{
    WebCore::ScreenProperties properties;
    properties.primaryDisplayID = primaryIdentifier;
    for (size_t i = 0; i < count; ++i) {
        const auto& screen = screens[i];
        if (!screen.identifier)
            continue;
        WebCore::ScreenData data;
        data.screenRect = WebCore::FloatRect(screen.frame.origin.x, screen.frame.origin.y, screen.frame.size.width, screen.frame.size.height);
        data.screenAvailableRect = WebCore::FloatRect(screen.availableFrame.origin.x, screen.availableFrame.origin.y, screen.availableFrame.size.width, screen.availableFrame.size.height);
        data.screenDepth = screen.depth;
        data.screenDepthPerComponent = screen.depthPerComponent;
        data.screenSize = WebCore::IntSize(screen.widthMillimeters, screen.heightMillimeters);
        data.dpi = screen.dpi;
        properties.screenDataMap.set(screen.identifier, WTF::move(data));
    }
    if (properties == WebKit::cutefishScreenProperties())
        return;
    WebKit::cutefishScreenProperties() = properties;
    for (auto& pool : WebKit::WebProcessPool::allProcessPools())
        pool->sendToAllProcesses(Messages::WebProcess::SetScreenProperties(properties));
}
