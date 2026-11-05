// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "CurlSSLHandle.h"

namespace WebCore {

void CurlSSLHandle::platformInitialize()
{
    // libcurl falls back to the system certificate store.
}

} // namespace WebCore
