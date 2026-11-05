// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include <wtf/glib/ChassisType.h>

namespace WTF {

ChassisType chassisType()
{
    return ChassisType::Desktop;
}

} // namespace WTF
