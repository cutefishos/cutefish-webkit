// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKContextSandbox.h"

#include "WKAPICast.h"
#include "WebProcessPool.h"
#include <wtf/text/CString.h>

void WKContextSetSandboxEnabled(WKContextRef context, bool enabled)
{
    auto& processPool = *WebKit::toImpl(context);
    RELEASE_ASSERT(processPool.processes().isEmpty());
    processPool.setSandboxEnabled(enabled);
}

bool WKContextGetSandboxEnabled(WKContextRef context)
{
    return WebKit::toImpl(context)->sandboxEnabled();
}

void WKContextAddPathToSandbox(WKContextRef context, const char* path, bool readOnly)
{
    auto& processPool = *WebKit::toImpl(context);
    RELEASE_ASSERT(processPool.processes().isEmpty());
    processPool.addSandboxPath(CString(path), readOnly ? WebKit::SandboxPermission::ReadOnly : WebKit::SandboxPermission::ReadWrite);
}
