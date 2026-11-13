// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKMainRunLoop.h"

#include <glib.h>
#include <sys/epoll.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/RunLoop.h>
#include <wtf/Seconds.h>
#include <wtf/Vector.h>
#include <wtf/unix/UnixFileDescriptor.h>

static constexpr size_t initialPollDescriptorCount = 16;

static UnixFileDescriptor& epollDescriptor()
{
    static NeverDestroyed<UnixFileDescriptor> descriptor;
    return descriptor;
}

static Vector<GPollFD>& watchedDescriptors()
{
    static NeverDestroyed<Vector<GPollFD>> descriptors;
    return descriptors;
}

static uint32_t epollEvents(gushort events)
{
    uint32_t result = 0;
    if (events & G_IO_IN)
        result |= EPOLLIN;
    if (events & G_IO_OUT)
        result |= EPOLLOUT;
    if (events & G_IO_PRI)
        result |= EPOLLPRI;
    return result;
}

// Puts the descriptors the default context polls next into the epoll set and
// returns its timeout in milliseconds.
static int watchNextIteration()
{
    auto* context = g_main_context_default();
    int maxPriority = 0;
    g_main_context_prepare(context, &maxPriority);

    Vector<GPollFD> descriptors(std::max(watchedDescriptors().size(), initialPollDescriptorCount));
    int timeout = 0;
    int count = 0;
    while ((count = g_main_context_query(context, maxPriority, &timeout, descriptors.mutableSpan().data(), descriptors.size())) > static_cast<int>(descriptors.size()))
        descriptors.grow(count);
    descriptors.shrink(count);

    int epoll = epollDescriptor().value();
    for (auto& watched : watchedDescriptors()) {
        bool stillPolled = descriptors.containsIf([&](auto& descriptor) {
            return descriptor.fd == watched.fd;
        });
        if (!stillPolled)
            epoll_ctl(epoll, EPOLL_CTL_DEL, watched.fd, nullptr);
    }
    // A closed descriptor leaves the epoll set by itself and its number may come
    // back for another file, so each one is modified, or added when missing.
    for (auto& descriptor : descriptors) {
        epoll_event event { };
        event.events = epollEvents(descriptor.events);
        event.data.fd = descriptor.fd;
        if (epoll_ctl(epoll, EPOLL_CTL_MOD, descriptor.fd, &event) == -1 && errno == ENOENT)
            epoll_ctl(epoll, EPOLL_CTL_ADD, descriptor.fd, &event);
    }
    watchedDescriptors() = WTF::move(descriptors);

    return timeout;
}

int WKMainRunLoopGetDescriptor()
{
    RELEASE_ASSERT(RunLoop::isMain());
    if (!epollDescriptor()) {
        epollDescriptor() = UnixFileDescriptor { epoll_create1(EPOLL_CLOEXEC), UnixFileDescriptor::Adopt };
        // GLib wakes a context for sources attached from other threads only
        // when a thread owns it.
        g_main_context_acquire(g_main_context_default());
        watchNextIteration();
    }
    return epollDescriptor().value();
}

double WKMainRunLoopIterate()
{
    WKMainRunLoopGetDescriptor();
    RunLoop::cycle();

    int timeout = watchNextIteration();
    if (timeout < 0)
        return -1;
    return Seconds::fromMilliseconds(timeout).seconds();
}

void WKMainRunLoopWakeUp()
{
    g_main_context_wakeup(g_main_context_default());
}
