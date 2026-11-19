// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKPagePDF_h
#define WKPagePDF_h

#include <WebKit/WKBase.h>
#include <WebKit/WKGeometry.h>

#ifdef __cplusplus
extern "C" {
#endif

// The PDF, null when the page could not draw one; it lives until the
// function returns.
typedef void (*WKPagePDFFunction)(WKDataRef pdf, void* context);

// The main frame's contents in a rect of document coordinates, or all of
// them when rect is null, as a one-page vector PDF.
WK_EXPORT void WKPageDrawContentsToPDF(WKPageRef, const WKRect* rect, void* context, WKPagePDFFunction);

// The main frame laid out for printing on paper of the given size, in
// points, with margins and a scale (1 for none), as a PDF of its pages.
typedef struct WKPagePrintLayout {
    double paperWidth;
    double paperHeight;
    double marginTop;
    double marginRight;
    double marginBottom;
    double marginLeft;
    double scale;
} WKPagePrintLayout;

WK_EXPORT void WKPageDrawPagesToPDFForPrinting(WKPageRef, const WKPagePrintLayout*, void* context, WKPagePDFFunction);

#ifdef __cplusplus
}
#endif

#endif // WKPagePDF_h
