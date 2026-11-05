/*
 * Copyright (C) 2025 Igalia S.L.
 * Copyright (C) 2026 Reion Wong <reionwong@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebPasteboardProxy.h"

#include "Connection.h"
#include "WKPasteboard.h"
#include <WebCore/Pasteboard.h>
#include <WebCore/PasteboardCustomData.h>
#include <WebCore/PasteboardItemInfo.h>
#include <WebCore/SelectionData.h>
#include <WebCore/SharedBuffer.h>
#include <wtf/HashMap.h>
#include <wtf/ListHashSet.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/text/CString.h>

namespace WebKit {
using namespace WebCore;

// The clipboard is held in the UI process; formats keep their write order.
// With a client, it caches the system pasteboard: formats are listed again
// when the client's change count moves, and contents are read on first use.
struct Clipboard {
    Vector<String> formats;
    HashMap<String, Ref<SharedBuffer>> contents;
    int64_t changeCount { 0 };
    std::optional<WKPasteboardClientV0> client;
    int64_t clientChangeCount { 0 };

    void clear()
    {
        formats.clear();
        contents.clear();
        changeCount++;
    }

    void set(const String& format, Ref<SharedBuffer>&& buffer)
    {
        if (!contents.contains(format))
            formats.append(format);
        contents.set(format, WTF::move(buffer));
    }

    void setString(const String& format, const String& string)
    {
        set(format, SharedBuffer::create(string.utf8().span()));
    }

    RefPtr<SharedBuffer> get(const String& format)
    {
        auto it = contents.find(format);
        if (it != contents.end())
            return it->value.ptr();
        if (!client || !formats.contains(format))
            return nullptr;

        Vector<uint8_t> bytes;
        bool found = false;
        auto context = std::make_pair(&bytes, &found);
        client->copyData(format.utf8().data(), [](const void* data, size_t length, void* context) {
            auto* target = static_cast<std::pair<Vector<uint8_t>*, bool*>*>(context);
            target->first->append(unsafeMakeSpan(static_cast<const uint8_t*>(data), length));
            *target->second = true;
        }, &context, client->base.clientInfo);
        if (!found)
            return nullptr;

        Ref buffer = SharedBuffer::create(WTF::move(bytes));
        contents.set(format, buffer.copyRef());
        return buffer;
    }

    void sync()
    {
        if (!client)
            return;
        int64_t count = client->changeCount(client->base.clientInfo);
        if (count == clientChangeCount)
            return;

        clientChangeCount = count;
        clear();
        client->copyTypes([](const char* type, void* context) {
            auto& formats = *static_cast<Vector<String>*>(context);
            auto format = String::fromUTF8(type);
            if (!format.isEmpty() && !formats.contains(format))
                formats.append(WTF::move(format));
        }, &formats, client->base.clientInfo);
    }

    void publish()
    {
        if (!client)
            return;

        Vector<CString> types;
        Vector<const char*> typePointers;
        Vector<const void*> bytes;
        Vector<size_t> lengths;
        for (auto& format : formats) {
            auto it = contents.find(format);
            if (it == contents.end())
                continue;
            types.append(format.utf8());
            bytes.append(it->value->span().data());
            lengths.append(it->value->size());
        }
        for (auto& type : types)
            typePointers.append(type.data());

        client->write(types.size(), typePointers.span().data(), bytes.span().data(), lengths.span().data(), client->base.clientInfo);
        clientChangeCount = client->changeCount(client->base.clientInfo);
    }
};

static Clipboard& clipboard()
{
    static NeverDestroyed<Clipboard> clipboard;
    clipboard->sync();
    return clipboard;
}

static String stringFromBuffer(const RefPtr<SharedBuffer>& buffer)
{
    return buffer ? String::fromUTF8(buffer->span()) : String();
}

static PasteboardItemInfo pasteboardItemInfoFromFormats(const Vector<String>& formats)
{
    PasteboardItemInfo info;
    if (formats.contains("text/plain"_s))
        info.webSafeTypesByFidelity.append("text/plain"_s);
    if (formats.contains("text/html"_s))
        info.webSafeTypesByFidelity.append("text/html"_s);
    if (formats.contains("text/uri-list"_s))
        info.webSafeTypesByFidelity.append("text/uri-list"_s);
    if (formats.contains("image/svg+xml"_s))
        info.webSafeTypesByFidelity.append("image/svg+xml"_s);
    info.platformTypesByFidelity = formats;
    return info;
}

void WebPasteboardProxy::getTypes(const String&, CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    completionHandler(Vector<String> { clipboard().formats });
}

void WebPasteboardProxy::readText(IPC::Connection&, const String&, const String& pasteboardType, CompletionHandler<void(String&&)>&& completionHandler)
{
    auto type = pasteboardType.startsWith("text/plain"_s) ? "text/plain"_s : pasteboardType;
    completionHandler(stringFromBuffer(clipboard().get(type)));
}

void WebPasteboardProxy::readFilePaths(IPC::Connection&, const String&, CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    completionHandler({ });
}

void WebPasteboardProxy::readBuffer(IPC::Connection&, const String&, const String& pasteboardType, CompletionHandler<void(RefPtr<SharedBuffer>&&)>&& completionHandler)
{
    completionHandler(clipboard().get(pasteboardType));
}

void WebPasteboardProxy::writeToClipboard(const String&, SelectionData&& selectionData)
{
    auto& board = clipboard();
    board.clear();
    if (selectionData.hasText())
        board.setString("text/plain"_s, selectionData.text());
    if (selectionData.hasMarkup())
        board.setString("text/html"_s, selectionData.markup());
    if (selectionData.hasURIList())
        board.setString("text/uri-list"_s, selectionData.uriList());
    board.publish();
}

void WebPasteboardProxy::clearClipboard(const String&)
{
    auto& board = clipboard();
    board.clear();
    board.publish();
}

void WebPasteboardProxy::typesSafeForDOMToReadAndWrite(IPC::Connection&, const String&, const String& origin, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    auto& board = clipboard();
    ListHashSet<String> domTypes;
    if (RefPtr buffer = board.get(PasteboardCustomData::cutefishType())) {
        auto customData = PasteboardCustomData::fromSharedBuffer(*buffer);
        if (customData.origin() == origin) {
            for (auto& type : customData.orderedTypes())
                domTypes.add(type);
        }
    }

    for (auto& format : board.formats) {
        if (format == PasteboardCustomData::cutefishType())
            continue;
        if (Pasteboard::isSafeTypeForDOMToReadAndWrite(format))
            domTypes.add(format);
    }
    completionHandler(copyToVector(domTypes));
}

void WebPasteboardProxy::writeCustomData(IPC::Connection&, const Vector<PasteboardCustomData>& data, const String&, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(int64_t)>&& completionHandler)
{
    auto& board = clipboard();
    // Only one item is kept, as on the other ports.
    if (data.size() != 1) {
        completionHandler(board.changeCount);
        return;
    }

    board.clear();
    const auto& customData = data[0];
    customData.forEachPlatformStringOrBuffer([&board](auto& type, auto& stringOrBuffer) {
        if (std::holds_alternative<Ref<SharedBuffer>>(stringOrBuffer))
            board.set(type, std::get<Ref<SharedBuffer>>(stringOrBuffer).copyRef());
        else if (std::holds_alternative<String>(stringOrBuffer))
            board.setString(type, std::get<String>(stringOrBuffer));
    });

    if (customData.hasSameOriginCustomData() || !customData.origin().isEmpty())
        board.set(PasteboardCustomData::cutefishType(), customData.createSharedBuffer());

    board.publish();
    completionHandler(board.changeCount);
}

void WebPasteboardProxy::allPasteboardItemInfo(IPC::Connection&, const String&, int64_t changeCount, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(std::optional<Vector<PasteboardItemInfo>>&&)>&& completionHandler)
{
    auto& board = clipboard();
    if (board.changeCount != changeCount) {
        completionHandler(std::nullopt);
        return;
    }
    completionHandler(Vector<PasteboardItemInfo> { pasteboardItemInfoFromFormats(board.formats) });
}

void WebPasteboardProxy::informationForItemAtIndex(IPC::Connection&, uint64_t index, const String&, int64_t changeCount, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(std::optional<PasteboardItemInfo>&&)>&& completionHandler)
{
    auto& board = clipboard();
    if (index || board.changeCount != changeCount) {
        completionHandler(std::nullopt);
        return;
    }
    completionHandler(pasteboardItemInfoFromFormats(board.formats));
}

void WebPasteboardProxy::getPasteboardItemsCount(IPC::Connection&, const String&, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(uint64_t)>&& completionHandler)
{
    completionHandler(clipboard().formats.isEmpty() ? 0 : 1);
}

void WebPasteboardProxy::readURLFromPasteboard(IPC::Connection&, uint64_t index, const String&, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(String&& url, String&& title)>&& completionHandler)
{
    if (index) {
        completionHandler({ }, { });
        return;
    }
    completionHandler(stringFromBuffer(clipboard().get("text/uri-list"_s)), { });
}

void WebPasteboardProxy::readBufferFromPasteboard(IPC::Connection&, std::optional<uint64_t> index, const String& pasteboardType, const String&, std::optional<WebPageProxyIdentifier>, CompletionHandler<void(RefPtr<SharedBuffer>&&)>&& completionHandler)
{
    if (index && *index) {
        completionHandler({ });
        return;
    }
    completionHandler(clipboard().get(pasteboardType));
}

void WebPasteboardProxy::getPasteboardChangeCount(IPC::Connection&, const String&, CompletionHandler<void(int64_t)>&& completionHandler)
{
    completionHandler(clipboard().changeCount);
}

} // namespace WebKit

void WKPasteboardSetClient(const WKPasteboardClientBase* client)
{
    auto& board = WebKit::clipboard();
    if (!client) {
        board.client = std::nullopt;
        return;
    }
    board.client = *reinterpret_cast<const WKPasteboardClientV0*>(client);
    board.clientChangeCount = board.client->changeCount(board.client->base.clientInfo) - 1;
}
