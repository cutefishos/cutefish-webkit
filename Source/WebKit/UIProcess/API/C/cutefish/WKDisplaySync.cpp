// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKDisplaySync.h"

#include "DisplayVBlankMonitorCutefish.h"

using namespace WebKit;

void WKDisplaySyncSetClient(const WKDisplaySyncClient* client)
{
    DisplayVBlankMonitorCutefish::setClient(client);
}

void WKDisplaySyncFired()
{
    DisplayVBlankMonitorCutefish::fired();
}
