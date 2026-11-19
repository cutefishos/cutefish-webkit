// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKPagePDF.h"

#include "APIData.h"
#include "PrintInfo.h"
#include "WKAPICast.h"
#include "WebFrameProxy.h"
#include "WebPageMessages.h"
#include "WebPageProxy.h"
#include "WebProcessProxy.h"
#include <WebCore/SharedBuffer.h>

using namespace WebKit;

static CompletionHandler<void(RefPtr<WebCore::SharedBuffer>&&)> pdfReply(void* context, WKPagePDFFunction function)
{
    return [context, function](RefPtr<WebCore::SharedBuffer>&& data) {
        if (!data) {
            function(nullptr, context);
            return;
        }
        Ref pdf = API::Data::create(data->span());
        function(toAPI(pdf.ptr()), context);
    };
}

void WKPageDrawContentsToPDF(WKPageRef pageRef, const WKRect* rect, void* context, WKPagePDFFunction function)
{
    Ref page = *toImpl(pageRef);
    std::optional<WebCore::FloatRect> contentsRect;
    if (rect)
        contentsRect = WebCore::FloatRect(rect->origin.x, rect->origin.y, rect->size.width, rect->size.height);
    if (!page->hasRunningProcess()) {
        function(nullptr, context);
        return;
    }
    page->legacyMainFrameProcess().sendWithAsyncReply(Messages::WebPage::DrawContentsToPDF(contentsRect), pdfReply(context, function), page->webPageIDInMainFrameProcess());
}

void WKPageDrawPagesToPDFForPrinting(WKPageRef pageRef, const WKPagePrintLayout* layout, void* context, WKPagePDFFunction function)
{
    Ref page = *toImpl(pageRef);
    RefPtr mainFrame = page->mainFrame();
    if (!page->hasRunningProcess() || !mainFrame || !layout) {
        function(nullptr, context);
        return;
    }
    WebCore::FloatBoxExtent margin(layout->marginTop, layout->marginRight, layout->marginBottom, layout->marginLeft);
    PrintInfo printInfo;
    printInfo.pageSetupScaleFactor = layout->scale > 0 ? layout->scale : 1;
    printInfo.availablePaperWidth = layout->paperWidth - layout->marginLeft - layout->marginRight;
    printInfo.availablePaperHeight = layout->paperHeight - layout->marginTop - layout->marginBottom;
    printInfo.margin = margin;
    page->legacyMainFrameProcess().sendWithAsyncReply(Messages::WebPage::DrawPagesToPDFForPrinting(mainFrame->frameID(), printInfo), pdfReply(context, function), page->webPageIDInMainFrameProcess());
}
