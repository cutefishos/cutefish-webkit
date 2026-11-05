// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "ProcessLauncher.h"

#include "IPCUtilities.h"
#include "ProcessExecutablePath.h"
#include <errno.h>
#include <signal.h>
#include <spawn.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <wtf/FileSystem.h>
#include <wtf/RunLoop.h>
#include <wtf/SafeStrerror.h>
#include <wtf/Threading.h>
#include <wtf/text/CString.h>
#include <wtf/text/MakeString.h>
#include <wtf/text/WTFString.h>

extern char** environ;

namespace WebKit {

void ProcessLauncher::launchProcess()
{
    String executablePath;
    switch (m_launchOptions.processType) {
    case ProcessLauncher::ProcessType::Web:
        executablePath = executablePathOfWebProcess();
        break;
    case ProcessLauncher::ProcessType::Network:
        executablePath = executablePathOfNetworkProcess();
        break;
#if ENABLE(GPU_PROCESS)
    case ProcessLauncher::ProcessType::GPU:
        executablePath = executablePathOfGPUProcess();
        break;
#endif
    default:
        ASSERT_NOT_REACHED();
        return;
    }

    // Both ends are close-on-exec so that no other child started at the same
    // time inherits the client end; the child gets it through a dup2() below,
    // which clears the flag on the copy.
    IPC::SocketPair socketPair = IPC::createPlatformConnection(SOCK_SEQPACKET, IPC::PlatformConnectionOptions::SetCloexecOnClient | IPC::PlatformConnectionOptions::SetCloexecOnServer);
    int clientSocket = socketPair.client.value();
    int childSocket = clientSocket == STDERR_FILENO + 1 ? STDERR_FILENO + 2 : STDERR_FILENO + 1;

    CString realExecutablePath = FileSystem::fileSystemRepresentation(executablePath);
    CString processIdentifier = String::number(m_launchOptions.processIdentifier.toUInt64()).utf8();
    CString childSocketString = String::number(childSocket).utf8();

    Vector<CString> prefixArguments;
#if ENABLE(DEVELOPER_MODE)
    if (!m_launchOptions.processCmdPrefix.isNull()) {
        for (auto& argument : m_launchOptions.processCmdPrefix.split(' '))
            prefixArguments.append(argument.utf8());
    }
#endif

    Vector<char*> argv;
    for (auto& argument : prefixArguments)
        argv.append(const_cast<char*>(argument.data()));
    argv.append(const_cast<char*>(realExecutablePath.data()));
    argv.append(const_cast<char*>(processIdentifier.data()));
    argv.append(const_cast<char*>(childSocketString.data()));
#if ENABLE(DEVELOPER_MODE)
    if (m_launchOptions.processType == ProcessLauncher::ProcessType::Web && m_client && m_client->shouldConfigureJSCForTesting())
        argv.append(const_cast<char*>("--configure-jsc-for-testing"));
#endif
    argv.append(nullptr);

    posix_spawn_file_actions_t fileActions;
    posix_spawn_file_actions_init(&fileActions);
    posix_spawn_file_actions_adddup2(&fileActions, clientSocket, childSocket);

    // The child starts with every signal at its default action and none blocked,
    // whatever the embedding program ignores or masks.
    posix_spawnattr_t attributes;
    posix_spawnattr_init(&attributes);
    sigset_t signals;
    sigfillset(&signals);
    posix_spawnattr_setsigdefault(&attributes, &signals);
    sigemptyset(&signals);
    posix_spawnattr_setsigmask(&attributes, &signals);
    posix_spawnattr_setflags(&attributes, POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK);

    pid_t processID = 0;
    int result = prefixArguments.isEmpty()
        ? posix_spawn(&processID, realExecutablePath.data(), &fileActions, &attributes, argv.span().data(), environ)
        : posix_spawnp(&processID, argv[0], &fileActions, &attributes, argv.span().data(), environ);

    posix_spawnattr_destroy(&attributes);
    posix_spawn_file_actions_destroy(&fileActions);
    socketPair.client = { };

    if (result) {
        WTFLogAlways("Unable to spawn %s: %s", realExecutablePath.data(), safeStrerror(result).data());
        RELEASE_ASSERT_NOT_REACHED();
    }

    m_processID = processID;

    // Nothing else reaps the children: the embedding program may not handle
    // SIGCHLD, and a handler installed here would take that from it.
    Thread::create("WebKit: Process reaper"_s, [processID] {
        while (waitpid(processID, nullptr, 0) == -1 && errno == EINTR) { }
    })->detach();

    RunLoop::mainSingleton().dispatch([protectedThis = protect(*this), this, serverSocket = WTF::move(socketPair.server)] mutable {
        didFinishLaunchingProcess(m_processID, IPC::Connection::Identifier { WTF::move(serverSocket) });
    });
}

void ProcessLauncher::terminateProcess()
{
    if (m_isLaunching) {
        invalidate();
        return;
    }

    if (!m_processID)
        return;

    kill(m_processID, SIGKILL);
    m_processID = 0;
}

void ProcessLauncher::platformInvalidate()
{
}

} // namespace WebKit
