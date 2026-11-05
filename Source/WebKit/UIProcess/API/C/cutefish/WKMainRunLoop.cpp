// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKMainRunLoop.h"

#include <wtf/MonotonicTime.h>
#include <wtf/RunLoop.h>

void WKMainRunLoopSetWakeUpCallback(WKMainRunLoopWakeUpCallback callback, void* userData)
{
    RELEASE_ASSERT(RunLoop::isMain());
    if (!callback) {
        RunLoop::setWakeUpCallback(nullptr);
        return;
    }
    RunLoop::setWakeUpCallback([callback, userData] {
        callback(userData);
    });
}

double WKMainRunLoopIterate()
{
    RunLoop::cycle();

    auto wakeUpTime = RunLoop::mainSingleton().nextWakeUpTime();
    if (!wakeUpTime)
        return -1;
    return std::max(0_s, *wakeUpTime - MonotonicTime::now()).seconds();
}
