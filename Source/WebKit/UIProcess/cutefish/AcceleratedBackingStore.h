// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "MessageReceiver.h"
#include "RendererBufferFormat.h"
#include <WebCore/IntRect.h>
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/TZoneMalloc.h>
#include <wtf/Vector.h>
#include <wtf/WeakPtr.h>
#include <wtf/unix/UnixFileDescriptor.h>

namespace WebCore {
class ShareableBitmap;
class ShareableBitmapHandle;
struct DMABufBufferAttributes;
}

namespace WebKit {

class CutefishWebView;
class WebProcessProxy;

// Receives the buffers the web process renders a page into and hands them,
// and each frame, to the view's client.
class AcceleratedBackingStore final : public IPC::MessageReceiver, public RefCounted<AcceleratedBackingStore> {
    WTF_MAKE_TZONE_ALLOCATED(AcceleratedBackingStore);
    WTF_MAKE_NONCOPYABLE(AcceleratedBackingStore);
public:
    using Rects = Vector<WebCore::IntRect, 1>;

    static Ref<AcceleratedBackingStore> create(CutefishWebView&);
    ~AcceleratedBackingStore();

    void ref() const final { RefCounted::ref(); }
    void deref() const final { RefCounted::deref(); }

    void updateSurfaceID(uint64_t);

    void frameDisplayed();
    void releaseBuffer(uint64_t id, UnixFileDescriptor&& releaseFence);

private:
    explicit AcceleratedBackingStore(CutefishWebView&);

    // IPC::MessageReceiver.
    void didReceiveMessage(IPC::Connection&, IPC::Decoder&) override;

    void didCreateDMABufBuffer(uint64_t id, WebCore::DMABufBufferAttributes&&, RendererBufferFormat::Usage);
    void didCreateSHMBuffer(uint64_t id, WebCore::ShareableBitmapHandle&&);
    void didDestroyBuffer(uint64_t id);
    void frame(uint64_t id, Rects&&, UnixFileDescriptor&& renderingFence);

    void frameDone();
    void destroyBuffers();

    CutefishWebView& m_view;
    uint64_t m_surfaceID { 0 };
    WeakPtr<WebProcessProxy> m_process;
    HashMap<uint64_t, RefPtr<WebCore::ShareableBitmap>> m_buffers;
    bool m_hasPendingFrame { false };
};

} // namespace WebKit
