// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WebProcessMain.h"

#include "AuxiliaryProcessMain.h"
#include "WebProcess.h"
#include <skia/core/SkGraphics.h>

#if USE(SKIA_OPENTYPE_SVG)
WTF_IGNORE_WARNINGS_IN_THIRD_PARTY_CODE_BEGIN
#include <skia/modules/svg/SkSVGOpenTypeSVGDecoder.h>
WTF_IGNORE_WARNINGS_IN_THIRD_PARTY_CODE_END
#endif

namespace WebKit {

class WebProcessMainCutefish final : public AuxiliaryProcessMainBase<WebProcess> {
public:
    bool platformInitialize() override
    {
        SkGraphics::Init();
#if USE(SKIA_OPENTYPE_SVG)
        SkGraphics::SetOpenTypeSVGDecoderFactory(SkSVGOpenTypeSVGDecoder::Make);
#endif
        return true;
    }
};

int WebProcessMain(int argc, char** argv)
{
    return AuxiliaryProcessMain<WebProcessMainCutefish>(argc, argv);
}

} // namespace WebKit
