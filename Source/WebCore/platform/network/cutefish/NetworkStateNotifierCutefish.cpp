// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "NetworkStateNotifier.h"

#include "NotImplemented.h"
#include <ifaddrs.h>
#include <net/if.h>

namespace WebCore {

void NetworkStateNotifier::updateStateWithoutNotifying()
{
    struct ifaddrs* addresses = nullptr;
    if (getifaddrs(&addresses)) {
        m_isOnLine = true;
        return;
    }

    bool isOnLine = false;
    for (auto* address = addresses; address; address = address->ifa_next) {
        if (!address->ifa_addr || (address->ifa_flags & IFF_LOOPBACK))
            continue;
        if ((address->ifa_flags & IFF_UP) && (address->ifa_flags & IFF_RUNNING)) {
            isOnLine = true;
            break;
        }
    }
    freeifaddrs(addresses);
    m_isOnLine = isOnLine;
}

void NetworkStateNotifier::startObserving()
{
    notImplemented();
}

} // namespace WebCore
