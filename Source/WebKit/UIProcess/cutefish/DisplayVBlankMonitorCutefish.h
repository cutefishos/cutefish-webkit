// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "DisplayVBlankMonitor.h"
#include "WKDisplaySync.h"
#include <wtf/Lock.h>

namespace WebKit {

// Refreshes the embedder reports through WKDisplaySync, so the pages
// render in step with the frames its windows present.
class DisplayVBlankMonitorCutefish final : public DisplayVBlankMonitor {
public:
    static std::unique_ptr<DisplayVBlankMonitor> create();
    explicit DisplayVBlankMonitorCutefish(unsigned refreshRate);
    ~DisplayVBlankMonitorCutefish();

    static void setClient(const WKDisplaySyncClient*);
    static void fired();

private:
    Type type() const override { return Type::Timer; }
    void start() override;
    void stop() override;
    bool isActive() override;
    void invalidate() override;

    bool m_active { false };
};

} // namespace WebKit
