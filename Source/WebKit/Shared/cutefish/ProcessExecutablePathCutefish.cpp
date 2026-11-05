// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "ProcessExecutablePath.h"

#include <wtf/FileSystem.h>

namespace WebKit {

static String findWebKitProcess(const ASCIILiteral processName)
{
#if ENABLE(DEVELOPER_MODE)
    // A build tree runs its processes from the directory of the program using it.
    static NeverDestroyed<String> executableDirectory = [] {
        CString executablePath = FileSystem::currentExecutablePath();
        if (executablePath.isNull())
            return String();
        return FileSystem::parentPath(FileSystem::stringFromFileSystemRepresentation(executablePath.data()));
    }();
    if (!executableDirectory->isNull()) {
        String processPath = FileSystem::pathByAppendingComponent(executableDirectory.get(), processName);
        if (FileSystem::fileExists(processPath))
            return processPath;
    }
#endif

    return FileSystem::pathByAppendingComponent(FileSystem::stringFromFileSystemRepresentation(PKGLIBEXECDIR), processName);
}

String executablePathOfWebProcess()
{
    return findWebKitProcess("CutefishWebProcess"_s);
}

String executablePathOfNetworkProcess()
{
    return findWebKitProcess("CutefishNetworkProcess"_s);
}

#if ENABLE(GPU_PROCESS)
String executablePathOfGPUProcess()
{
    return findWebKitProcess("CutefishGPUProcess"_s);
}
#endif

} // namespace WebKit
