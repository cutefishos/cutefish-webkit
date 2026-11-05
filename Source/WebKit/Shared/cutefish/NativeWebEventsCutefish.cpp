// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "NativeWebTouchEvent.h"
#include "NativeWebWheelEvent.h"

namespace WebKit {

NativeWebKeyboardEvent::NativeWebKeyboardEvent(WebKeyboardEvent&& event)
    : WebKeyboardEvent(WTF::move(event))
{
}

NativeWebMouseEvent::NativeWebMouseEvent(WebMouseEvent&& event)
    : WebMouseEvent(WTF::move(event))
{
}

NativeWebWheelEvent::NativeWebWheelEvent(WebWheelEvent&& event)
    : WebWheelEvent(WTF::move(event))
{
}

#if ENABLE(TOUCH_EVENTS)
NativeWebTouchEvent::NativeWebTouchEvent(WebTouchEvent&& event)
    : WebTouchEvent(WTF::move(event))
{
}
#endif

} // namespace WebKit
