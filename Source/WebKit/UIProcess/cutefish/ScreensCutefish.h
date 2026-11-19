// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <WebCore/ScreenProperties.h>

namespace WebKit {

// What the embedder last told of the desktop's screens.
WebCore::ScreenProperties& cutefishScreenProperties();

} // namespace WebKit
