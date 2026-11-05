// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WebsiteDataStore.h"

#include <mutex>
#include <wtf/FileSystem.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/text/CString.h>

namespace WebKit {

static String programName()
{
    auto name = FileSystem::currentExecutableName();
    if (!name.isNull() && name.length())
        return FileSystem::stringFromFileSystemRepresentation(name.data());
    return "cutefish-webkit"_s;
}

static const String& defaultBaseCacheDirectory()
{
    static NeverDestroyed<String> baseCacheDirectory;
    static std::once_flag once;
    std::call_once(once, [] {
        baseCacheDirectory.get() = FileSystem::pathByAppendingComponent(FileSystem::userCacheDirectory(), programName());
    });
    return baseCacheDirectory;
}

static const String& defaultBaseDataDirectory()
{
    static NeverDestroyed<String> baseDataDirectory;
    static std::once_flag once;
    std::call_once(once, [] {
        baseDataDirectory.get() = FileSystem::pathByAppendingComponent(FileSystem::userDataDirectory(), programName());
    });
    return baseDataDirectory;
}

String WebsiteDataStore::cacheDirectoryFileSystemRepresentation(const String& directoryName, const String& baseCacheDirectory, ShouldCreateDirectory)
{
    return FileSystem::pathByAppendingComponent(baseCacheDirectory.isNull() ? defaultBaseCacheDirectory() : baseCacheDirectory, directoryName);
}

String WebsiteDataStore::websiteDataDirectoryFileSystemRepresentation(const String& directoryName, const String& baseDataDirectory, ShouldCreateDirectory)
{
    return FileSystem::pathByAppendingComponent(baseDataDirectory.isNull() ? defaultBaseDataDirectory() : baseDataDirectory, directoryName);
}

UnifiedOriginStorageLevel WebsiteDataStore::defaultUnifiedOriginStorageLevel()
{
    return UnifiedOriginStorageLevel::Basic;
}

void WebsiteDataStore::platformInitialize()
{
}

void WebsiteDataStore::platformDestroy()
{
}

} // namespace WebKit
