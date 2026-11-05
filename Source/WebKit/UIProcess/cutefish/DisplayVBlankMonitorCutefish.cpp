// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "DisplayVBlankMonitorCutefish.h"

#include <wtf/MainThread.h>
#include <wtf/Vector.h>

namespace WebKit {

static Lock s_lock;
static std::optional<WKDisplaySyncClient> s_client WTF_GUARDED_BY_LOCK(s_lock);
static Vector<DisplayVBlankMonitorCutefish*> s_monitors WTF_GUARDED_BY_LOCK(s_lock);
static unsigned s_activeCount WTF_GUARDED_BY_LOCK(s_lock);

void DisplayVBlankMonitorCutefish::setClient(const WKDisplaySyncClient* client)
{
    Locker locker { s_lock };
    if (client)
        s_client = *client;
    else
        s_client = std::nullopt;
}

std::unique_ptr<DisplayVBlankMonitor> DisplayVBlankMonitorCutefish::create()
{
    unsigned refreshRate;
    {
        Locker locker { s_lock };
        if (!s_client)
            return nullptr;
        refreshRate = s_client->refreshRate ? s_client->refreshRate / 1000 : 60;
    }
    return makeUnique<DisplayVBlankMonitorCutefish>(refreshRate);
}

DisplayVBlankMonitorCutefish::DisplayVBlankMonitorCutefish(unsigned refreshRate)
    : DisplayVBlankMonitor(refreshRate)
{
    Locker locker { s_lock };
    s_monitors.append(this);
}

DisplayVBlankMonitorCutefish::~DisplayVBlankMonitorCutefish()
{
    invalidate();
}

// The client's calls run on the main thread, where it keeps its windows;
// display links may start and stop their monitors from others. The client
// runs while any monitor is active.
void DisplayVBlankMonitorCutefish::start()
{
    {
        Locker locker { s_lock };
        if (m_active || !s_monitors.contains(this) || !s_client)
            return;
        m_active = true;
        if (s_activeCount++)
            return;
    }
    callOnMainRunLoop([] {
        Locker locker { s_lock };
        if (s_client && s_activeCount)
            s_client->start(s_client->context);
    });
}

void DisplayVBlankMonitorCutefish::stop()
{
    {
        Locker locker { s_lock };
        if (!m_active)
            return;
        m_active = false;
        if (--s_activeCount)
            return;
    }
    callOnMainRunLoop([] {
        Locker locker { s_lock };
        if (s_client && !s_activeCount)
            s_client->stop(s_client->context);
    });
}

bool DisplayVBlankMonitorCutefish::isActive()
{
    Locker locker { s_lock };
    return m_active;
}

void DisplayVBlankMonitorCutefish::invalidate()
{
    stop();
    Locker locker { s_lock };
    s_monitors.removeFirst(this);
}

void DisplayVBlankMonitorCutefish::fired()
{
    Vector<DisplayVBlankMonitorCutefish*> active;
    {
        Locker locker { s_lock };
        for (auto* monitor : s_monitors) {
            if (monitor->m_active)
                active.append(monitor);
        }
    }
    for (auto* monitor : active) {
        if (monitor->m_handler)
            monitor->m_handler();
    }
}

} // namespace WebKit
