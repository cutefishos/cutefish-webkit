// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "InjectedBundle.h"

#include "WKBundleAPICast.h"
#include "WKBundleInitialize.h"
#include <dlfcn.h>
#include <wtf/FileSystem.h>
#include <wtf/text/CString.h>

namespace WebKit {

bool InjectedBundle::initialize(const WebProcessCreationParameters&, RefPtr<API::Object>&& initializationUserData)
{
    m_platformBundle = dlopen(FileSystem::fileSystemRepresentation(m_path).data(), RTLD_NOW | RTLD_LOCAL);
    if (!m_platformBundle) {
        WTFLogAlways("Error loading the injected bundle (%s): %s", m_path.utf8().data(), dlerror());
        return false;
    }

    auto initializeFunction = reinterpret_cast<WKBundleInitializeFunctionPtr>(dlsym(m_platformBundle, "WKBundleInitialize"));
    if (!initializeFunction) {
        WTFLogAlways("Error loading WKBundleInitialize symbol from injected bundle.");
        return false;
    }

    initializeFunction(toAPI(this), toAPI(initializationUserData.get()));
    return true;
}

void InjectedBundle::setBundleParameter(const String&, std::span<const uint8_t>)
{
}

void InjectedBundle::setBundleParameters(std::span<const uint8_t>)
{
}

} // namespace WebKit
