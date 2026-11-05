// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "AcceleratedBackingStore.h"

#include "AcceleratedBackingStoreMessages.h"
#include "AcceleratedSurfaceMessages.h"
#include "CutefishWebView.h"
#include "WKAPICast.h"
#include "WebPageProxy.h"
#include "WebProcessProxy.h"
#include <WebCore/DMABufBufferAttributes.h>
#include <WebCore/ShareableBitmap.h>
#include <drm_fourcc.h>
#include <unistd.h>
#include <wtf/TZoneMallocInlines.h>

namespace WebKit {

WTF_MAKE_TZONE_ALLOCATED_IMPL(AcceleratedBackingStore);

Ref<AcceleratedBackingStore> AcceleratedBackingStore::create(CutefishWebView& view)
{
    return adoptRef(*new AcceleratedBackingStore(view));
}

AcceleratedBackingStore::AcceleratedBackingStore(CutefishWebView& view)
    : m_view(view)
{
}

AcceleratedBackingStore::~AcceleratedBackingStore()
{
    updateSurfaceID(0);
}

void AcceleratedBackingStore::updateSurfaceID(uint64_t surfaceID)
{
    if (m_surfaceID == surfaceID)
        return;

    if (m_surfaceID) {
        if (m_hasPendingFrame)
            frameDone();
        destroyBuffers();
        if (RefPtr process = m_process.get())
            process->removeMessageReceiver(Messages::AcceleratedBackingStore::messageReceiverName(), m_surfaceID);
        m_process = nullptr;
    }

    m_surfaceID = surfaceID;
    if (m_surfaceID) {
        Ref process = m_view.page().legacyMainFrameProcess();
        process->addMessageReceiver(Messages::AcceleratedBackingStore::messageReceiverName(), m_surfaceID, *this);
        m_process = process.get();
    }
}

void AcceleratedBackingStore::destroyBuffers()
{
    auto& client = m_view.client();
    for (auto id : m_buffers.keys()) {
        if (client.bufferDestroyed)
            client.bufferDestroyed(toAPI(&m_view), id, client.base.clientInfo);
    }
    m_buffers.clear();
}

void AcceleratedBackingStore::didCreateDMABufBuffer(uint64_t id, WebCore::DMABufBufferAttributes&& attributes, RendererBufferFormat::Usage)
{
    WKViewBuffer buffer { };
    buffer.identifier = id;
    buffer.type = kWKViewBufferTypeDMABuf;
    buffer.width = attributes.size.width();
    buffer.height = attributes.size.height();
    buffer.fourcc = attributes.fourcc.value;
    buffer.modifier = attributes.modifier;
    buffer.planeCount = std::min<uint32_t>(attributes.fds.size(), std::size(buffer.fds));
    for (uint32_t plane = 0; plane < buffer.planeCount; ++plane) {
        buffer.fds[plane] = attributes.fds[plane].release();
        buffer.offsets[plane] = attributes.offsets[plane];
        buffer.strides[plane] = attributes.strides[plane];
    }
    m_buffers.add(id, nullptr);

    auto& client = m_view.client();
    if (client.bufferCreated)
        client.bufferCreated(toAPI(&m_view), &buffer, client.base.clientInfo);
    else {
        for (uint32_t plane = 0; plane < buffer.planeCount; ++plane)
            close(buffer.fds[plane]);
    }
}

void AcceleratedBackingStore::didCreateSHMBuffer(uint64_t id, WebCore::ShareableBitmap::Handle&& handle)
{
    auto bitmap = WebCore::ShareableBitmap::create(WTF::move(handle), WebCore::SharedMemory::Protection::ReadOnly);
    if (!bitmap)
        return;

    WKViewBuffer buffer { };
    buffer.identifier = id;
    buffer.type = kWKViewBufferTypeSharedMemory;
    buffer.width = bitmap->size().width();
    buffer.height = bitmap->size().height();
    buffer.fourcc = DRM_FORMAT_ARGB8888;
    buffer.planeCount = 1;
    buffer.fds[0] = -1;
    buffer.strides[0] = bitmap->bytesPerRow();
    buffer.data = bitmap->span().data();
    buffer.size = bitmap->span().size();
    m_buffers.add(id, WTF::move(bitmap));

    auto& client = m_view.client();
    if (client.bufferCreated)
        client.bufferCreated(toAPI(&m_view), &buffer, client.base.clientInfo);
}

void AcceleratedBackingStore::didDestroyBuffer(uint64_t id)
{
    if (!m_buffers.remove(id))
        return;

    auto& client = m_view.client();
    if (client.bufferDestroyed)
        client.bufferDestroyed(toAPI(&m_view), id, client.base.clientInfo);
}

void AcceleratedBackingStore::frame(uint64_t id, Rects&& damage, UnixFileDescriptor&& renderingFence)
{
    auto& client = m_view.client();
    if (!m_buffers.contains(id) || !client.frame) {
        frameDone();
        return;
    }

    Vector<WKRect, 1> rects;
    rects.reserveInitialCapacity(damage.size());
    for (auto& rect : damage)
        rects.append(toAPI(rect));

    m_hasPendingFrame = true;
    client.frame(toAPI(&m_view), id, rects.span().data(), rects.size(), renderingFence.release(), client.base.clientInfo);
}

void AcceleratedBackingStore::frameDisplayed()
{
    if (m_hasPendingFrame)
        frameDone();
}

void AcceleratedBackingStore::frameDone()
{
    m_hasPendingFrame = false;
    if (RefPtr process = m_process.get())
        process->send(Messages::AcceleratedSurface::FrameDone(), m_surfaceID);
}

void AcceleratedBackingStore::releaseBuffer(uint64_t id, UnixFileDescriptor&& releaseFence)
{
    if (!m_buffers.contains(id))
        return;

    if (RefPtr process = m_process.get())
        process->send(Messages::AcceleratedSurface::ReleaseBuffer(id, WTF::move(releaseFence)), m_surfaceID);
}

} // namespace WebKit
