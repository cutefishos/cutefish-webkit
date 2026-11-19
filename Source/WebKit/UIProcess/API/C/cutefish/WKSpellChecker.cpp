// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKSpellChecker.h"

#include "SpellCheckerClientCutefish.h"
#include "TextChecker.h"
#include "TextCheckerState.h"

namespace WebKit {

static std::optional<WKSpellCheckerClient>& client()
{
    static std::optional<WKSpellCheckerClient> client;
    return client;
}

const WKSpellCheckerClient* spellCheckerClient()
{
    return client() ? &*client() : nullptr;
}

} // namespace WebKit

void WKSpellCheckerSetClient(const WKSpellCheckerClient* client)
{
    if (client)
        WebKit::client() = *client;
    else
        WebKit::client() = std::nullopt;
}

void WKSpellCheckerSetContinuousSpellCheckingEnabled(bool enabled)
{
    WebKit::TextChecker::setContinuousSpellCheckingEnabled(enabled);
}

bool WKSpellCheckerGetContinuousSpellCheckingEnabled()
{
    return WebKit::TextChecker::state().contains(WebKit::TextCheckerState::ContinuousSpellCheckingEnabled);
}
