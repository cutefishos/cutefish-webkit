// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include <wtf/FileSystem.h>

#include <array>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <wtf/text/CString.h>
#include <wtf/text/MakeString.h>
#include <wtf/text/WTFString.h>

namespace WTF {

namespace FileSystemImpl {

String filenameForDisplay(const String& string)
{
    return string;
}

CString currentExecutablePath()
{
    static std::array<char, PATH_MAX> readLinkBuffer;
    ssize_t result = readlink("/proc/self/exe", readLinkBuffer.data(), readLinkBuffer.size());
    if (result <= 0)
        return { };
    return CString(unsafeMakeSpan(readLinkBuffer.data(), static_cast<size_t>(result)));
}

CString currentExecutableName()
{
    auto executablePath = currentExecutablePath();
    if (executablePath.isNull())
        return { };
    return FileSystem::fileSystemRepresentation(FileSystem::pathFileName(FileSystem::stringFromFileSystemRepresentation(executablePath.data())));
}

static String homeDirectory()
{
    auto* entry = getpwuid(getuid());
    if (!entry || !entry->pw_dir)
        return { };
    return stringFromFileSystemRepresentation(entry->pw_dir);
}

String userCacheDirectory()
{
    return pathByAppendingComponent(homeDirectory(), ".cache"_s);
}

String userDataDirectory()
{
    std::array<StringView, 2> components { ".local"_s, "share"_s };
    return pathByAppendingComponents(homeDirectory(), components);
}

String createTemporaryDirectory(const String& directoryPrefix)
{
    CString directoryTemplate = fileSystemRepresentation(makeString(directoryPrefix, "XXXXXX"_s));
    Vector<char> buffer(directoryTemplate.span());
    buffer.append('\0');
    if (!mkdtemp(buffer.mutableSpan().data()))
        return { };
    return stringFromFileSystemRepresentation(buffer.span().data());
}

} // namespace FileSystemImpl

} // namespace WTF
