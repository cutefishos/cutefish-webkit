// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#ifndef WTF_PLATFORM_GUARD_AGAINST_INDIRECT_INCLUSION
#error "Please #include <wtf/Platform.h> instead of this file directly."
#endif

#if !PLATFORM(CUTEFISH)
#error "This file should only be included when building the Cutefish port."
#endif

/* Please keep the following in alphabetical order so we can notice duplicates. */
/* Items should only be here if they are different from the defaults in PlatformEnable.h. */

#if !defined(ENABLE_KINETIC_SCROLLING) && ENABLE(ASYNC_SCROLLING)
#define ENABLE_KINETIC_SCROLLING 1
#endif

#if !defined(ENABLE_OPENTYPE_VERTICAL)
#define ENABLE_OPENTYPE_VERTICAL 1
#endif

#if !defined(ENABLE_SCROLLING_THREAD) && USE(COORDINATED_GRAPHICS)
#define ENABLE_SCROLLING_THREAD 1
#endif

#if !defined(ENABLE_WHEEL_EVENT_REGIONS)
#define ENABLE_WHEEL_EVENT_REGIONS 1
#endif
