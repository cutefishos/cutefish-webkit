// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKEventCutefish_h
#define WKEventCutefish_h

#include <WebKit/WKBase.h>
#include <WebKit/WKEvent.h>
#include <WebKit/WKGeometry.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    kWKEventMouseDown,
    kWKEventMouseUp,
    kWKEventMouseMove,

    kWKEventKeyDown,
    kWKEventKeyUp,

    kWKEventTouchStart,
    kWKEventTouchMove,
    kWKEventTouchEnd,
    kWKEventTouchCancel
};
typedef int32_t WKEventType;

enum {
    kWKEventMouseButtonsLeft = 1 << 0,
    kWKEventMouseButtonsRight = 1 << 1,
    kWKEventMouseButtonsMiddle = 1 << 2,
    kWKEventMouseButtonsBack = 1 << 3,
    kWKEventMouseButtonsForward = 1 << 4
};
typedef uint16_t WKEventMouseButtons;

// Positions are in view coordinates, in logical pixels; timestamps are
// seconds on the monotonic clock.
typedef struct WKMouseEvent {
    WKEventType type;
    WKEventMouseButton button;
    WKEventMouseButtons buttons;
    WKPoint position;
    WKPoint globalPosition;
    int32_t clickCount;
    WKEventModifiers modifiers;
    double timestamp;
} WKMouseEvent;

enum {
    kWKWheelEventPhaseNone,
    kWKWheelEventPhaseBegan,
    kWKWheelEventPhaseChanged,
    kWKWheelEventPhaseEnded,
    kWKWheelEventPhaseCancelled
};
typedef uint8_t WKWheelEventPhase;

// delta is in logical pixels, positive towards the end of the content;
// wheelTicks counts detents of a stepped wheel and stays zero for a touchpad.
typedef struct WKWheelEvent {
    WKPoint position;
    WKPoint globalPosition;
    WKSize delta;
    WKSize wheelTicks;
    WKWheelEventPhase phase;
    WKWheelEventPhase momentumPhase;
    bool hasPreciseDeltas;
    WKEventModifiers modifiers;
    double timestamp;
} WKWheelEvent;

// keysym and keycode follow xkbcommon: an xkb_keysym_t and an xkb_keycode_t
// (evdev code plus 8). text is the UTF-8 the key produces, or null.
typedef struct WKKeyboardEvent {
    WKEventType type;
    uint32_t keysym;
    uint32_t keycode;
    const char* text;
    bool isAutoRepeat;
    bool isKeypad;
    WKEventModifiers modifiers;
    double timestamp;
} WKKeyboardEvent;

enum {
    kWKTouchPointPressed,
    kWKTouchPointMoved,
    kWKTouchPointReleased,
    kWKTouchPointStationary,
    kWKTouchPointCancelled
};
typedef uint8_t WKTouchPointState;

typedef struct WKTouchPoint {
    int32_t id;
    WKTouchPointState state;
    WKPoint position;
    WKPoint globalPosition;
} WKTouchPoint;

typedef struct WKTouchEvent {
    WKEventType type;
    const WKTouchPoint* points;
    size_t pointCount;
    WKEventModifiers modifiers;
    double timestamp;
} WKTouchEvent;

#ifdef __cplusplus
}
#endif

#endif /* WKEventCutefish_h */
