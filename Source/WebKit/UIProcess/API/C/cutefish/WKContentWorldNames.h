// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "APIContentWorld.h"
#include "WKAPICast.h"
#include <wtf/HashMap.h>
#include <wtf/NeverDestroyed.h>

namespace WebKit {

// A named world lives as long as the embedder: the embedder names it again
// to reach the same globals, and has no object of its own to keep it.
inline API::ContentWorld& contentWorldNamed(WKStringRef world)
{
    if (!world)
        return API::ContentWorld::pageContentWorldSingleton();
    String name = toWTFString(world);
    if (name.isEmpty())
        return API::ContentWorld::defaultClientWorldSingleton();
    static NeverDestroyed<HashMap<String, Ref<API::ContentWorld>>> worlds;
    return worlds->ensure(name, [&] {
        return API::ContentWorld::sharedWorldWithName(name);
    }).iterator->value.get();
}

} // namespace WebKit
