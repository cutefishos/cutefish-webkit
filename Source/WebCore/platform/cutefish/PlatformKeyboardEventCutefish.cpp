/*
 * Copyright (C) 2014 Igalia S.L.
 * Copyright (C) 2026 Reion Wong <reionwong@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PlatformKeyboardEvent.h"

#if PLATFORM(CUTEFISH)
#include "WindowsKeyboardCodes.h"
#include <xkbcommon/xkbcommon.h>
#include <wtf/HexNumber.h>
#include <wtf/text/MakeString.h>
#include <wtf/text/StringBuilder.h>
#endif

namespace WebCore {

#if PLATFORM(CUTEFISH)
// FIXME: This is incomplete. We should change this to mirror
// more like what Firefox does, and generate these switch statements
// at build time.
// https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values
String PlatformKeyboardEvent::keyValueForKeysym(unsigned keyCode)
{
    switch (keyCode) {
    // Modifier keys.
    case XKB_KEY_Alt_L:
    case XKB_KEY_Alt_R:
        return "Alt"_s;
    // Firefox uses XKB_KEY_Mode_switch for AltGraph as well.
    case XKB_KEY_ISO_Level3_Shift:
    case XKB_KEY_ISO_Level3_Latch:
    case XKB_KEY_ISO_Level3_Lock:
    case XKB_KEY_ISO_Level5_Shift:
    case XKB_KEY_ISO_Level5_Latch:
    case XKB_KEY_ISO_Level5_Lock:
        return "AltGraph"_s;
    case XKB_KEY_Caps_Lock:
        return "CapsLock"_s;
    case XKB_KEY_Control_L:
    case XKB_KEY_Control_R:
        return "Control"_s;
    // Fn: This is typically a hardware key that does not generate a separate code.
    // FnLock.
    case XKB_KEY_Hyper_L:
    case XKB_KEY_Hyper_R:
        return "Hyper"_s;
    case XKB_KEY_Meta_L:
    case XKB_KEY_Meta_R:
        return "Meta"_s;
    case XKB_KEY_Num_Lock:
        return "NumLock"_s;
    case XKB_KEY_Scroll_Lock:
        return "ScrollLock"_s;
    case XKB_KEY_Shift_L:
    case XKB_KEY_Shift_R:
        return "Shift"_s;
    case XKB_KEY_Super_L:
    case XKB_KEY_Super_R:
        return "Super"_s;
    // Symbol.
    // SymbolLock.

    // Whitespace keys.
    case XKB_KEY_Return:
    case XKB_KEY_KP_Enter:
    case XKB_KEY_ISO_Enter:
    case XKB_KEY_3270_Enter:
        return "Enter"_s;
    case XKB_KEY_Tab:
    case XKB_KEY_KP_Tab:
        return "Tab"_s;

    // Navigation keys.
    case XKB_KEY_Down:
    case XKB_KEY_KP_Down:
        return "ArrowDown"_s;
    case XKB_KEY_Left:
    case XKB_KEY_KP_Left:
        return "ArrowLeft"_s;
    case XKB_KEY_Right:
    case XKB_KEY_KP_Right:
        return "ArrowRight"_s;
    case XKB_KEY_Up:
    case XKB_KEY_KP_Up:
        return "ArrowUp"_s;
    case XKB_KEY_End:
    case XKB_KEY_KP_End:
        return "End"_s;
    case XKB_KEY_Home:
    case XKB_KEY_KP_Home:
        return "Home"_s;
    case XKB_KEY_Page_Down:
    case XKB_KEY_KP_Page_Down:
        return "PageDown"_s;
    case XKB_KEY_Page_Up:
    case XKB_KEY_KP_Page_Up:
        return "PageUp"_s;

    // Editing keys.
    case XKB_KEY_BackSpace:
        return "Backspace"_s;
    case XKB_KEY_Clear:
        return "Clear"_s;
    case XKB_KEY_XF86Copy:
        return "Copy"_s;
    case XKB_KEY_3270_CursorSelect:
        return "CrSel"_s;
    case XKB_KEY_XF86Cut:
        return "Cut"_s;
    case XKB_KEY_Delete:
    case XKB_KEY_KP_Delete:
        return "Delete"_s;
    case XKB_KEY_3270_EraseEOF:
        return "EraseEof"_s;
    case XKB_KEY_3270_ExSelect:
        return "ExSel"_s;
    case XKB_KEY_Insert:
    case XKB_KEY_KP_Insert:
        return "Insert"_s;
    case XKB_KEY_XF86Paste:
        return "Paste"_s;
    case XKB_KEY_Redo:
        return "Redo"_s;
    case XKB_KEY_Undo:
        return "Undo"_s;

    // UI keys.
    // Accept.
    // Again.
    case XKB_KEY_3270_Attn:
        return "Attn"_s;
    case XKB_KEY_Cancel:
        return "Cancel"_s;
    case XKB_KEY_Menu:
        return "ContextMenu"_s;
    case XKB_KEY_Escape:
        return "Escape"_s;
    case XKB_KEY_Execute:
        return "Execute"_s;
    case XKB_KEY_Find:
        return "Find"_s;
    case XKB_KEY_Help:
        return "Help"_s;
    case XKB_KEY_Pause:
    case XKB_KEY_Break:
        return "Pause"_s;
    case XKB_KEY_3270_Play:
        return "Play"_s;
    // Props.
    case XKB_KEY_Select:
        return "Select"_s;
    case XKB_KEY_XF86ZoomIn:
        return "ZoomIn"_s;
    case XKB_KEY_XF86ZoomOut:
        return "ZoomOut"_s;

    // Device keys.
    case XKB_KEY_XF86MonBrightnessDown:
        return "BrightnessDown"_s;
    case XKB_KEY_XF86MonBrightnessUp:
        return "BrightnessUp"_s;
    case XKB_KEY_XF86Eject:
        return "Eject"_s;
    case XKB_KEY_XF86LogOff:
        return "LogOff"_s;
    // Power.
    case XKB_KEY_XF86PowerDown:
    case XKB_KEY_XF86PowerOff:
        return "PowerOff"_s;
    case XKB_KEY_3270_PrintScreen:
    case XKB_KEY_Print:
    case XKB_KEY_Sys_Req:
        return "PrintScreen"_s;
    case XKB_KEY_XF86Hibernate:
        return "Hibernate"_s;
    case XKB_KEY_XF86Standby:
    case XKB_KEY_XF86Suspend:
    case XKB_KEY_XF86Sleep:
        return "Standby"_s;
    case XKB_KEY_XF86WakeUp:
        return "WakeUp"_s;

    // IME keys.
    case XKB_KEY_MultipleCandidate:
        return "AllCandidates"_s;
    case XKB_KEY_Eisu_Shift:
    case XKB_KEY_Eisu_toggle:
        return "Alphanumeric"_s;
    case XKB_KEY_Codeinput:
        return "CodeInput"_s;
    case XKB_KEY_Multi_key:
        return "Compose"_s;
    case XKB_KEY_Henkan:
        return "Convert"_s;
    case XKB_KEY_dead_grave:
    case XKB_KEY_dead_acute:
    case XKB_KEY_dead_circumflex:
    case XKB_KEY_dead_tilde:
    case XKB_KEY_dead_macron:
    case XKB_KEY_dead_breve:
    case XKB_KEY_dead_abovedot:
    case XKB_KEY_dead_diaeresis:
    case XKB_KEY_dead_abovering:
    case XKB_KEY_dead_doubleacute:
    case XKB_KEY_dead_caron:
    case XKB_KEY_dead_cedilla:
    case XKB_KEY_dead_ogonek:
    case XKB_KEY_dead_iota:
    case XKB_KEY_dead_voiced_sound:
    case XKB_KEY_dead_semivoiced_sound:
    case XKB_KEY_dead_belowdot:
    case XKB_KEY_dead_hook:
    case XKB_KEY_dead_horn:
    case XKB_KEY_dead_stroke:
    case XKB_KEY_dead_abovecomma:
    case XKB_KEY_dead_abovereversedcomma:
    case XKB_KEY_dead_doublegrave:
    case XKB_KEY_dead_belowring:
    case XKB_KEY_dead_belowmacron:
    case XKB_KEY_dead_belowcircumflex:
    case XKB_KEY_dead_belowtilde:
    case XKB_KEY_dead_belowbreve:
    case XKB_KEY_dead_belowdiaeresis:
    case XKB_KEY_dead_invertedbreve:
    case XKB_KEY_dead_belowcomma:
    case XKB_KEY_dead_currency:
    case XKB_KEY_dead_a:
    case XKB_KEY_dead_A:
    case XKB_KEY_dead_e:
    case XKB_KEY_dead_E:
    case XKB_KEY_dead_i:
    case XKB_KEY_dead_I:
    case XKB_KEY_dead_o:
    case XKB_KEY_dead_O:
    case XKB_KEY_dead_u:
    case XKB_KEY_dead_U:
    case XKB_KEY_dead_small_schwa:
    case XKB_KEY_dead_capital_schwa:
        return "Dead"_s;
    // FinalMode
    case XKB_KEY_ISO_First_Group:
        return "GroupFirst"_s;
    case XKB_KEY_ISO_Last_Group:
        return "GroupLast"_s;
    case XKB_KEY_ISO_Next_Group:
        return "GroupNext"_s;
    case XKB_KEY_ISO_Prev_Group:
        return "GroupPrevious"_s;
    case XKB_KEY_Mode_switch:
        return "ModeChange"_s;
    // NextCandidate.
    case XKB_KEY_Muhenkan:
        return "NonConvert"_s;
    case XKB_KEY_PreviousCandidate:
        return "PreviousCandidate"_s;
    // Process.
    case XKB_KEY_SingleCandidate:
        return "SingleCandidate"_s;

    // Korean and Japanese keys.
    case XKB_KEY_Hangul:
        return "HangulMode"_s;
    case XKB_KEY_Hangul_Hanja:
        return "HanjaMode"_s;
    case XKB_KEY_Hangul_Jeonja:
        return "JunjaMode"_s;
    case XKB_KEY_Hankaku:
        return "Hankaku"_s;
    case XKB_KEY_Hiragana:
        return "Hiragana"_s;
    case XKB_KEY_Hiragana_Katakana:
        return "HiraganaKatakana"_s;
    case XKB_KEY_Kana_Lock:
    case XKB_KEY_Kana_Shift:
        return "KanaMode"_s;
    case XKB_KEY_Kanji:
        return "KanjiMode"_s;
    case XKB_KEY_Katakana:
        return "Katakana"_s;
    case XKB_KEY_Romaji:
        return "Romaji"_s;
    case XKB_KEY_Zenkaku:
        return "Zenkaku"_s;
    case XKB_KEY_Zenkaku_Hankaku:
        return "ZenkakuHanaku"_s;

    // Application Keys
    case XKB_KEY_XF86AudioMedia:
        return "LaunchMediaPlayer"_s;

    // Browser Keys
    case XKB_KEY_XF86Back:
        return "BrowserBack"_s;
    case XKB_KEY_XF86Favorites:
        return "BrowserFavorites"_s;
    case XKB_KEY_XF86Forward:
        return "BrowserForward"_s;
    case XKB_KEY_XF86HomePage:
        return "BrowserHome"_s;
    case XKB_KEY_XF86Refresh:
        return "BrowserRefresh"_s;
    case XKB_KEY_XF86Search:
        return "BrowserSearch"_s;
    case XKB_KEY_XF86Stop:
        return "BrowserStop"_s;

    // ChannelDown.
    // ChannelUp.
    case XKB_KEY_XF86Close:
        return "Close"_s;
    case XKB_KEY_XF86MailForward:
        return "MailForward"_s;
    case XKB_KEY_XF86Reply:
        return "MailReply"_s;
    case XKB_KEY_XF86Send:
        return "MailSend"_s;
    case XKB_KEY_XF86AudioForward:
        return "MediaFastForward"_s;
    case XKB_KEY_XF86AudioPause:
        return "MediaPause"_s;
    case XKB_KEY_XF86AudioPlay:
        return "MediaPlay"_s;
    // MediaPlayPause
    case XKB_KEY_XF86AudioRecord:
        return "MediaRecord"_s;
    case XKB_KEY_XF86AudioRewind:
        return "MediaRewind"_s;
    case XKB_KEY_XF86AudioStop:
        return "MediaStop"_s;
    case XKB_KEY_XF86AudioNext:
        return "MediaTrackNext"_s;
    case XKB_KEY_XF86AudioPrev:
        return "MediaTrackPrevious"_s;
    case XKB_KEY_XF86New:
        return "New"_s;
    case XKB_KEY_XF86Open:
        return "Open"_s;
    case XKB_KEY_XF86AudioLowerVolume:
        return "AudioVolumeDown"_s;
    case XKB_KEY_XF86AudioRaiseVolume:
        return "AudioVolumeUp"_s;
    case XKB_KEY_XF86AudioMute:
        return "AudioVolumeMute"_s;

    // Media Controller Keys
    case XKB_KEY_XF86Red:
        return "ColorF0Red"_s;
    case XKB_KEY_XF86Green:
        return "ColorF1Green"_s;
    case XKB_KEY_XF86Yellow:
        return "ColorF2Yellow"_s;
    case XKB_KEY_XF86Blue:
        return "ColorF3Blue"_s;
    case XKB_KEY_XF86Display:
        return "DisplaySwap"_s;
    case XKB_KEY_XF86Video:
        return "OnDemand"_s;
    case XKB_KEY_XF86Subtitle:
        return "Subtitle"_s;

    // Print.
    case XKB_KEY_XF86Save:
        return "Save"_s;
    case XKB_KEY_XF86Spell:
        return "SpellCheck"_s;

    // Function keys.
    case XKB_KEY_F1:
        return "F1"_s;
    case XKB_KEY_F2:
        return "F2"_s;
    case XKB_KEY_F3:
        return "F3"_s;
    case XKB_KEY_F4:
        return "F4"_s;
    case XKB_KEY_F5:
        return "F5"_s;
    case XKB_KEY_F6:
        return "F6"_s;
    case XKB_KEY_F7:
        return "F7"_s;
    case XKB_KEY_F8:
        return "F8"_s;
    case XKB_KEY_F9:
        return "F9"_s;
    case XKB_KEY_F10:
        return "F10"_s;
    case XKB_KEY_F11:
        return "F11"_s;
    case XKB_KEY_F12:
        return "F12"_s;
    case XKB_KEY_F13:
        return "F13"_s;
    case XKB_KEY_F14:
        return "F14"_s;
    case XKB_KEY_F15:
        return "F15"_s;
    case XKB_KEY_F16:
        return "F16"_s;
    case XKB_KEY_F17:
        return "F17"_s;
    case XKB_KEY_F18:
        return "F18"_s;
    case XKB_KEY_F19:
        return "F19"_s;
    case XKB_KEY_F20:
        return "F20"_s;
    default:
        break;
    }

    char32_t unicodeCharacter = xkb_keysym_to_utf32(keyCode);
    if (unicodeCharacter && U_IS_UNICODE_CHAR(unicodeCharacter))
        return makeString(unicodeCharacter);

    return "Unidentified"_s;
}

// FIXME: This is incomplete. We should change this to mirror
// more like what Firefox does, and generate these switch statements
// at build time.
// https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/code
String PlatformKeyboardEvent::keyCodeForHardwareKeyCode(unsigned keyCode)
{
    switch (keyCode) {
    case 0x0009:
        return "Escape"_s;
    case 0x000A:
        return "Digit1"_s;
    case 0x000B:
        return "Digit2"_s;
    case 0x000C:
        return "Digit3"_s;
    case 0x000D:
        return "Digit4"_s;
    case 0x000E:
        return "Digit5"_s;
    case 0x000F:
        return "Digit6"_s;
    case 0x0010:
        return "Digit7"_s;
    case 0x0011:
        return "Digit8"_s;
    case 0x0012:
        return "Digit9"_s;
    case 0x0013:
        return "Digit0"_s;
    case 0x0014:
        return "Minus"_s;
    case 0x0015:
        return "Equal"_s;
    case 0x0016:
        return "Backspace"_s;
    case 0x0017:
        return "Tab"_s;
    case 0x0018:
        return "KeyQ"_s;
    case 0x0019:
        return "KeyW"_s;
    case 0x001A:
        return "KeyE"_s;
    case 0x001B:
        return "KeyR"_s;
    case 0x001C:
        return "KeyT"_s;
    case 0x001D:
        return "KeyY"_s;
    case 0x001E:
        return "KeyU"_s;
    case 0x001F:
        return "KeyI"_s;
    case 0x0020:
        return "KeyO"_s;
    case 0x0021:
        return "KeyP"_s;
    case 0x0022:
        return "BracketLeft"_s;
    case 0x0023:
        return "BracketRight"_s;
    case 0x0024:
        return "Enter"_s;
    case 0x0025:
        return "ControlLeft"_s;
    case 0x0026:
        return "KeyA"_s;
    case 0x0027:
        return "KeyS"_s;
    case 0x0028:
        return "KeyD"_s;
    case 0x0029:
        return "KeyF"_s;
    case 0x002A:
        return "KeyG"_s;
    case 0x002B:
        return "KeyH"_s;
    case 0x002C:
        return "KeyJ"_s;
    case 0x002D:
        return "KeyK"_s;
    case 0x002E:
        return "KeyL"_s;
    case 0x002F:
        return "Semicolon"_s;
    case 0x0030:
        return "Quote"_s;
    case 0x0031:
        return "Backquote"_s;
    case 0x0032:
        return "ShiftLeft"_s;
    case 0x0033:
        return "Backslash"_s;
    case 0x0034:
        return "KeyZ"_s;
    case 0x0035:
        return "KeyX"_s;
    case 0x0036:
        return "KeyC"_s;
    case 0x0037:
        return "KeyV"_s;
    case 0x0038:
        return "KeyB"_s;
    case 0x0039:
        return "KeyN"_s;
    case 0x003A:
        return "KeyM"_s;
    case 0x003B:
        return "Comma"_s;
    case 0x003C:
        return "Period"_s;
    case 0x003D:
        return "Slash"_s;
    case 0x003E:
        return "ShiftRight"_s;
    case 0x003F:
        return "NumpadMultiply"_s;
    case 0x0040:
        return "AltLeft"_s;
    case 0x0041:
        return "Space"_s;
    case 0x0042:
        return "CapsLock"_s;
    case 0x0043:
        return "F1"_s;
    case 0x0044:
        return "F2"_s;
    case 0x0045:
        return "F3"_s;
    case 0x0046:
        return "F4"_s;
    case 0x0047:
        return "F5"_s;
    case 0x0048:
        return "F6"_s;
    case 0x0049:
        return "F7"_s;
    case 0x004A:
        return "F8"_s;
    case 0x004B:
        return "F9"_s;
    case 0x004C:
        return "F10"_s;
    case 0x004D:
        return "NumLock"_s;
    case 0x004E:
        return "ScrollLock"_s;
    case 0x004F:
        return "Numpad7"_s;
    case 0x0050:
        return "Numpad8"_s;
    case 0x0051:
        return "Numpad9"_s;
    case 0x0052:
        return "NumpadSubtract"_s;
    case 0x0053:
        return "Numpad4"_s;
    case 0x0054:
        return "Numpad5"_s;
    case 0x0055:
        return "Numpad6"_s;
    case 0x0056:
        return "NumpadAdd"_s;
    case 0x0057:
        return "Numpad1"_s;
    case 0x0058:
        return "Numpad2"_s;
    case 0x0059:
        return "Numpad3"_s;
    case 0x005A:
        return "Numpad0"_s;
    case 0x005B:
        return "NumpadDecimal"_s;
    case 0x005E:
        return "IntlBackslash"_s;
    case 0x005F:
        return "F11"_s;
    case 0x0060:
        return "F12"_s;
    case 0x0061:
        return "IntlRo"_s;
    case 0x0064:
        return "Convert"_s;
    case 0x0065:
        return "KanaMode"_s;
    case 0x0066:
        return "NonConvert"_s;
    case 0x0068:
        return "NumpadEnter"_s;
    case 0x0069:
        return "ControlRight"_s;
    case 0x006A:
        return "NumpadDivide"_s;
    case 0x006B:
        return "PrintScreen"_s;
    case 0x006C:
        return "AltRight"_s;
    case 0x006E:
        return "Home"_s;
    case 0x006F:
        return "ArrowUp"_s;
    case 0x0070:
        return "PageUp"_s;
    case 0x0071:
        return "ArrowLeft"_s;
    case 0x0072:
        return "ArrowRight"_s;
    case 0x0073:
        return "End"_s;
    case 0x0074:
        return "ArrowDown"_s;
    case 0x0075:
        return "PageDown"_s;
    case 0x0076:
        return "Insert"_s;
    case 0x0077:
        return "Delete"_s;
    case 0x0079:
        return "AudioVolumeMute"_s;
    case 0x007A:
        return "AudioVolumeDown"_s;
    case 0x007B:
        return "AudioVolumeUp"_s;
    case 0x007D:
        return "NumpadEqual"_s;
    case 0x007F:
        return "Pause"_s;
    case 0x0081:
        return "NumpadComma"_s;
    case 0x0082:
        return "Lang1"_s;
    case 0x0083:
        return "Lang2"_s;
    case 0x0084:
        return "IntlYen"_s;
    case 0x0085:
        return "OSLeft"_s;
    case 0x0086:
        return "OSRight"_s;
    case 0x0087:
        return "ContextMenu"_s;
    case 0x0088:
        return "BrowserStop"_s;
    case 0x0089:
        return "Again"_s;
    case 0x008A:
        return "Props"_s;
    case 0x008B:
        return "Undo"_s;
    case 0x008C:
        return "Select"_s;
    case 0x008D:
        return "Copy"_s;
    case 0x008E:
        return "Open"_s;
    case 0x008F:
        return "Paste"_s;
    case 0x0090:
        return "Find"_s;
    case 0x0091:
        return "Cut"_s;
    case 0x0092:
        return "Help"_s;
    case 0x0094:
        return "LaunchApp2"_s;
    case 0x0097:
        return "WakeUp"_s;
    case 0x0098:
        return "LaunchApp1"_s;
    case 0x00A3:
        return "LaunchMail"_s;
    case 0x00A4:
        return "BrowserFavorites"_s;
    case 0x00A6:
        return "BrowserBack"_s;
    case 0x00A7:
        return "BrowserForward"_s;
    case 0x00A9:
        return "Eject"_s;
    case 0x00AB:
        return "MediaTrackNext"_s;
    case 0x00AC:
        return "MediaPlayPause"_s;
    case 0x00AD:
        return "MediaTrackPrevious"_s;
    case 0x00AE:
        return "MediaStop"_s;
    case 0x00B3:
        return "LaunchMediaPlayer"_s;
    case 0x00B4:
        return "BrowserHome"_s;
    case 0x00B5:
        return "BrowserRefresh"_s;
    case 0x00BF:
        return "F13"_s;
    case 0x00C0:
        return "F14"_s;
    case 0x00C1:
        return "F15"_s;
    case 0x00C2:
        return "F16"_s;
    case 0x00C3:
        return "F17"_s;
    case 0x00C4:
        return "F18"_s;
    case 0x00C5:
        return "F19"_s;
    case 0x00C6:
        return "F20"_s;
    case 0x00C7:
        return "F21"_s;
    case 0x00C8:
        return "F22"_s;
    case 0x00C9:
        return "F23"_s;
    case 0x00CA:
        return "F24"_s;
    case 0x00E1:
        return "BrowserSearch"_s;
    case 0x00E7:
        return "Cancel"_s;
    case 0x00F8:
        return "Unknown"_s;
    case 0x016B:
        return "Clear"_s;
    default:
        break;
    }
    return "Unidentified"_s;
}

// FIXME: This is incomplete. We should change this to mirror
// more like what Firefox does, and generate these switch statements
// at build time.
String PlatformKeyboardEvent::keyIdentifierForKeysym(unsigned keyCode)
{
    switch (keyCode) {
    case XKB_KEY_Menu:
    case XKB_KEY_Alt_L:
    case XKB_KEY_Alt_R:
        return "Alt"_s;
    case XKB_KEY_Clear:
        return "Clear"_s;
    case XKB_KEY_Down:
    case XKB_KEY_KP_Down:
        return "Down"_s;
        // "End"
    case XKB_KEY_End:
    case XKB_KEY_KP_End:
        return "End"_s;
        // "Enter"
    case XKB_KEY_ISO_Enter:
    case XKB_KEY_KP_Enter:
    case XKB_KEY_Return:
        return "Enter"_s;
    case XKB_KEY_Execute:
        return "Execute"_s;
    case XKB_KEY_F1:
        return "F1"_s;
    case XKB_KEY_F2:
        return "F2"_s;
    case XKB_KEY_F3:
        return "F3"_s;
    case XKB_KEY_F4:
        return "F4"_s;
    case XKB_KEY_F5:
        return "F5"_s;
    case XKB_KEY_F6:
        return "F6"_s;
    case XKB_KEY_F7:
        return "F7"_s;
    case XKB_KEY_F8:
        return "F8"_s;
    case XKB_KEY_F9:
        return "F9"_s;
    case XKB_KEY_F10:
        return "F10"_s;
    case XKB_KEY_F11:
        return "F11"_s;
    case XKB_KEY_F12:
        return "F12"_s;
    case XKB_KEY_F13:
        return "F13"_s;
    case XKB_KEY_F14:
        return "F14"_s;
    case XKB_KEY_F15:
        return "F15"_s;
    case XKB_KEY_F16:
        return "F16"_s;
    case XKB_KEY_F17:
        return "F17"_s;
    case XKB_KEY_F18:
        return "F18"_s;
    case XKB_KEY_F19:
        return "F19"_s;
    case XKB_KEY_F20:
        return "F20"_s;
    case XKB_KEY_F21:
        return "F21"_s;
    case XKB_KEY_F22:
        return "F22"_s;
    case XKB_KEY_F23:
        return "F23"_s;
    case XKB_KEY_F24:
        return "F24"_s;
    case XKB_KEY_Help:
        return "Help"_s;
    case XKB_KEY_Home:
    case XKB_KEY_KP_Home:
        return "Home"_s;
    case XKB_KEY_Insert:
        return "Insert"_s;
    case XKB_KEY_Left:
    case XKB_KEY_KP_Left:
        return "Left"_s;
    case XKB_KEY_Page_Down:
    case XKB_KEY_KP_Page_Down:
        return "PageDown"_s;
    case XKB_KEY_Page_Up:
    case XKB_KEY_KP_Page_Up:
        return "PageUp"_s;
    case XKB_KEY_Pause:
        return "Pause"_s;
    case XKB_KEY_3270_PrintScreen:
    case XKB_KEY_Print:
        return "PrintScreen"_s;
    case XKB_KEY_Right:
    case XKB_KEY_KP_Right:
        return "Right"_s;
    case XKB_KEY_Select:
        return "Select"_s;
    case XKB_KEY_Up:
    case XKB_KEY_KP_Up:
        return "Up"_s;
        // Standard says that DEL becomes U+007F.
    case XKB_KEY_Delete:
        return "U+007F"_s;
    case XKB_KEY_BackSpace:
        return "U+0008"_s;
    case XKB_KEY_ISO_Left_Tab:
    case XKB_KEY_3270_BackTab:
    case XKB_KEY_Tab:
        return "U+0009"_s;
    default:
        break;
    }

    return makeString("U+"_s, hex(xkb_keysym_to_utf32(keyCode), 4));
}

int PlatformKeyboardEvent::windowsKeyCodeForKeysym(unsigned keycode)
{
    switch (keycode) {
    case XKB_KEY_Cancel:
        return 0x03; // (03) The Cancel key
    case XKB_KEY_KP_0:
        return VK_NUMPAD0; // (60) Numeric keypad 0 key
    case XKB_KEY_KP_1:
        return VK_NUMPAD1; // (61) Numeric keypad 1 key
    case XKB_KEY_KP_2:
        return VK_NUMPAD2; // (62) Numeric keypad 2 key
    case XKB_KEY_KP_3:
        return VK_NUMPAD3; // (63) Numeric keypad 3 key
    case XKB_KEY_KP_4:
        return VK_NUMPAD4; // (64) Numeric keypad 4 key
    case XKB_KEY_KP_5:
        return VK_NUMPAD5; // (65) Numeric keypad 5 key
    case XKB_KEY_KP_6:
        return VK_NUMPAD6; // (66) Numeric keypad 6 key
    case XKB_KEY_KP_7:
        return VK_NUMPAD7; // (67) Numeric keypad 7 key
    case XKB_KEY_KP_8:
        return VK_NUMPAD8; // (68) Numeric keypad 8 key
    case XKB_KEY_KP_9:
        return VK_NUMPAD9; // (69) Numeric keypad 9 key
    case XKB_KEY_KP_Multiply:
        return VK_MULTIPLY; // (6A) Multiply key
    case XKB_KEY_KP_Add:
        return VK_ADD; // (6B) Add key
    case XKB_KEY_KP_Subtract:
        return VK_SUBTRACT; // (6D) Subtract key
    case XKB_KEY_KP_Decimal:
        return VK_DECIMAL; // (6E) Decimal key
    case XKB_KEY_KP_Divide:
        return VK_DIVIDE; // (6F) Divide key

    case XKB_KEY_KP_Page_Up:
        return VK_PRIOR; // (21) PAGE UP key
    case XKB_KEY_KP_Page_Down:
        return VK_NEXT; // (22) PAGE DOWN key
    case XKB_KEY_KP_End:
        return VK_END; // (23) END key
    case XKB_KEY_KP_Home:
        return VK_HOME; // (24) HOME key
    case XKB_KEY_KP_Left:
        return VK_LEFT; // (25) LEFT ARROW key
    case XKB_KEY_KP_Up:
        return VK_UP; // (26) UP ARROW key
    case XKB_KEY_KP_Right:
        return VK_RIGHT; // (27) RIGHT ARROW key
    case XKB_KEY_KP_Down:
        return VK_DOWN; // (28) DOWN ARROW key

    case XKB_KEY_BackSpace:
        return VK_BACK; // (08) BACKSPACE key
    case XKB_KEY_ISO_Left_Tab:
    case XKB_KEY_3270_BackTab:
    case XKB_KEY_Tab:
        return VK_TAB; // (09) TAB key
    case XKB_KEY_Clear:
        return VK_CLEAR; // (0C) CLEAR key
    case XKB_KEY_ISO_Enter:
    case XKB_KEY_KP_Enter:
    case XKB_KEY_Return:
        return VK_RETURN; // (0D) Return key

        // VK_SHIFT (10) SHIFT key
        // VK_CONTROL (11) CTRL key

    case XKB_KEY_Menu:
        return VK_APPS; // (5D) Applications key (Natural keyboard)

        // VK_MENU (12) ALT key

    case XKB_KEY_Pause:
    case XKB_KEY_XF86AudioPause:
        return VK_PAUSE; // (13) PAUSE key
    case XKB_KEY_Caps_Lock:
        return VK_CAPITAL; // (14) CAPS LOCK key
    case XKB_KEY_Kana_Lock:
    case XKB_KEY_Kana_Shift:
        return VK_KANA; // (15) Input Method Editor (IME) Kana mode
    case XKB_KEY_Hangul:
        return VK_HANGUL; // VK_HANGUL (15) IME Hangul mode
        // VK_JUNJA (17) IME Junja mode
        // VK_FINAL (18) IME final mode
    case XKB_KEY_Hangul_Hanja:
        return VK_HANJA; // (19) IME Hanja mode
    case XKB_KEY_Kanji:
        return VK_KANJI; // (19) IME Kanji mode
    case XKB_KEY_Escape:
        return VK_ESCAPE; // (1B) ESC key
        // VK_CONVERT (1C) IME convert
        // VK_NONCONVERT (1D) IME nonconvert
        // VK_ACCEPT (1E) IME accept
        // VK_MODECHANGE (1F) IME mode change request
    case XKB_KEY_space:
        return VK_SPACE; // (20) SPACEBAR
    case XKB_KEY_Page_Up:
        return VK_PRIOR; // (21) PAGE UP key
    case XKB_KEY_Page_Down:
        return VK_NEXT; // (22) PAGE DOWN key
    case XKB_KEY_End:
        return VK_END; // (23) END key
    case XKB_KEY_Home:
        return VK_HOME; // (24) HOME key
    case XKB_KEY_Left:
        return VK_LEFT; // (25) LEFT ARROW key
    case XKB_KEY_Up:
        return VK_UP; // (26) UP ARROW key
    case XKB_KEY_Right:
        return VK_RIGHT; // (27) RIGHT ARROW key
    case XKB_KEY_Down:
        return VK_DOWN; // (28) DOWN ARROW key
    case XKB_KEY_Select:
        return VK_SELECT; // (29) SELECT key
    case XKB_KEY_Print:
        return VK_SNAPSHOT; // (2C) PRINT SCREEN key
    case XKB_KEY_Execute:
        return VK_EXECUTE; // (2B) EXECUTE key
    case XKB_KEY_Insert:
    case XKB_KEY_KP_Insert:
        return VK_INSERT; // (2D) INS key
    case XKB_KEY_Delete:
    case XKB_KEY_KP_Delete:
        return VK_DELETE; // (2E) DEL key
    case XKB_KEY_Help:
        return VK_HELP; // (2F) HELP key
    case XKB_KEY_0:
    case XKB_KEY_parenright:
        return VK_0; // (30) 0) key
    case XKB_KEY_1:
    case XKB_KEY_exclam:
        return VK_1; // (31) 1 ! key
    case XKB_KEY_2:
    case XKB_KEY_at:
        return VK_2; // (32) 2 & key
    case XKB_KEY_3:
    case XKB_KEY_numbersign:
        return VK_3; // case '3': case '#';
    case XKB_KEY_4:
    case XKB_KEY_dollar: // (34) 4 key '$';
        return VK_4;
    case XKB_KEY_5:
    case XKB_KEY_percent:
        return VK_5; // (35) 5 key  '%'
    case XKB_KEY_6:
    case XKB_KEY_asciicircum:
        return VK_6; // (36) 6 key  '^'
    case XKB_KEY_7:
    case XKB_KEY_ampersand:
        return VK_7; // (37) 7 key  case '&'
    case XKB_KEY_8:
    case XKB_KEY_asterisk:
        return VK_8; // (38) 8 key  '*'
    case XKB_KEY_9:
    case XKB_KEY_parenleft:
        return VK_9; // (39) 9 key '('
    case XKB_KEY_a:
    case XKB_KEY_A:
        return VK_A; // (41) A key case 'a': case 'A': return 0x41;
    case XKB_KEY_b:
    case XKB_KEY_B:
        return VK_B; // (42) B key case 'b': case 'B': return 0x42;
    case XKB_KEY_c:
    case XKB_KEY_C:
        return VK_C; // (43) C key case 'c': case 'C': return 0x43;
    case XKB_KEY_d:
    case XKB_KEY_D:
        return VK_D; // (44) D key case 'd': case 'D': return 0x44;
    case XKB_KEY_e:
    case XKB_KEY_E:
        return VK_E; // (45) E key case 'e': case 'E': return 0x45;
    case XKB_KEY_f:
    case XKB_KEY_F:
        return VK_F; // (46) F key case 'f': case 'F': return 0x46;
    case XKB_KEY_g:
    case XKB_KEY_G:
        return VK_G; // (47) G key case 'g': case 'G': return 0x47;
    case XKB_KEY_h:
    case XKB_KEY_H:
        return VK_H; // (48) H key case 'h': case 'H': return 0x48;
    case XKB_KEY_i:
    case XKB_KEY_I:
        return VK_I; // (49) I key case 'i': case 'I': return 0x49;
    case XKB_KEY_j:
    case XKB_KEY_J:
        return VK_J; // (4A) J key case 'j': case 'J': return 0x4A;
    case XKB_KEY_k:
    case XKB_KEY_K:
        return VK_K; // (4B) K key case 'k': case 'K': return 0x4B;
    case XKB_KEY_l:
    case XKB_KEY_L:
        return VK_L; // (4C) L key case 'l': case 'L': return 0x4C;
    case XKB_KEY_m:
    case XKB_KEY_M:
        return VK_M; // (4D) M key case 'm': case 'M': return 0x4D;
    case XKB_KEY_n:
    case XKB_KEY_N:
        return VK_N; // (4E) N key case 'n': case 'N': return 0x4E;
    case XKB_KEY_o:
    case XKB_KEY_O:
        return VK_O; // (4F) O key case 'o': case 'O': return 0x4F;
    case XKB_KEY_p:
    case XKB_KEY_P:
        return VK_P; // (50) P key case 'p': case 'P': return 0x50;
    case XKB_KEY_q:
    case XKB_KEY_Q:
        return VK_Q; // (51) Q key case 'q': case 'Q': return 0x51;
    case XKB_KEY_r:
    case XKB_KEY_R:
        return VK_R; // (52) R key case 'r': case 'R': return 0x52;
    case XKB_KEY_s:
    case XKB_KEY_S:
        return VK_S; // (53) S key case 's': case 'S': return 0x53;
    case XKB_KEY_t:
    case XKB_KEY_T:
        return VK_T; // (54) T key case 't': case 'T': return 0x54;
    case XKB_KEY_u:
    case XKB_KEY_U:
        return VK_U; // (55) U key case 'u': case 'U': return 0x55;
    case XKB_KEY_v:
    case XKB_KEY_V:
        return VK_V; // (56) V key case 'v': case 'V': return 0x56;
    case XKB_KEY_w:
    case XKB_KEY_W:
        return VK_W; // (57) W key case 'w': case 'W': return 0x57;
    case XKB_KEY_x:
    case XKB_KEY_X:
        return VK_X; // (58) X key case 'x': case 'X': return 0x58;
    case XKB_KEY_y:
    case XKB_KEY_Y:
        return VK_Y; // (59) Y key case 'y': case 'Y': return 0x59;
    case XKB_KEY_z:
    case XKB_KEY_Z:
        return VK_Z; // (5A) Z key case 'z': case 'Z': return 0x5A;
    case XKB_KEY_Meta_L:
        return VK_LWIN; // (5B) Left Windows key (Microsoft Natural keyboard)
    case XKB_KEY_Meta_R:
        return VK_RWIN; // (5C) Right Windows key (Natural keyboard)
    case XKB_KEY_XF86Sleep:
        return VK_SLEEP; // (5F) Computer Sleep key
        // VK_SEPARATOR (6C) Separator key
        // VK_SUBTRACT (6D) Subtract key
        // VK_DECIMAL (6E) Decimal key
        // VK_DIVIDE (6F) Divide key
        // handled by key code above

    case XKB_KEY_Num_Lock:
        return VK_NUMLOCK; // (90) NUM LOCK key

    case XKB_KEY_Scroll_Lock:
        return VK_SCROLL; // (91) SCROLL LOCK key

    case XKB_KEY_Shift_L:
        return VK_LSHIFT; // (A0) Left SHIFT key
    case XKB_KEY_Shift_R:
        return VK_RSHIFT; // (A1) Right SHIFT key
    case XKB_KEY_Control_L:
        return VK_LCONTROL; // (A2) Left CONTROL key
    case XKB_KEY_Control_R:
        return VK_RCONTROL; // (A3) Right CONTROL key
    case XKB_KEY_Alt_L:
        return VK_LMENU; // (A4) Left MENU key
    case XKB_KEY_Alt_R:
        return VK_RMENU; // (A5) Right MENU key

    case XKB_KEY_XF86Back:
        return VK_BROWSER_BACK; // VK_BROWSER_BACK (A6) Windows 2000/XP: Browser Back key
    case XKB_KEY_XF86Forward:
        return VK_BROWSER_FORWARD; // (A7) Windows 2000/XP: Browser Forward key
    case XKB_KEY_XF86Refresh:
        return VK_BROWSER_REFRESH; // (A8) Windows 2000/XP: Browser Refresh key
    case XKB_KEY_XF86Stop:
        return VK_BROWSER_STOP; // (A9) Windows 2000/XP: Browser Stop key
    case XKB_KEY_XF86Search:
        return VK_BROWSER_SEARCH; // (AA) Windows 2000/XP: Browser Search key
    case XKB_KEY_XF86Favorites:
        return VK_BROWSER_FAVORITES; // (AB) Windows 2000/XP: Browser Favorites key
    case XKB_KEY_XF86HomePage:
        return VK_BROWSER_HOME; // (AC) Windows 2000/XP: Browser Start and Home key
    case XKB_KEY_XF86AudioMute:
        return VK_VOLUME_MUTE; // (AD) Windows 2000/XP: Volume Mute key
    case XKB_KEY_XF86AudioLowerVolume:
        return VK_VOLUME_DOWN; // (AE) Windows 2000/XP: Volume Down key
    case XKB_KEY_XF86AudioRaiseVolume:
        return VK_VOLUME_UP; // (AF) Windows 2000/XP: Volume Up key
    case XKB_KEY_XF86AudioNext:
        return VK_MEDIA_NEXT_TRACK; // (B0) Windows 2000/XP: Next Track key
    case XKB_KEY_XF86AudioPrev:
        return VK_MEDIA_PREV_TRACK; // (B1) Windows 2000/XP: Previous Track key
    case XKB_KEY_XF86AudioStop:
        return VK_MEDIA_STOP; // (B2) Windows 2000/XP: Stop Media key
        // VK_MEDIA_PLAY_PAUSE (B3) Windows 2000/XP: Play/Pause Media key
        // VK_LAUNCH_MAIL (B4) Windows 2000/XP: Start Mail key
    case XKB_KEY_XF86AudioMedia:
        return VK_MEDIA_LAUNCH_MEDIA_SELECT; // (B5) Windows 2000/XP: Select Media key
        // VK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
        // VK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key

        // VK_OEM_1 (BA) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key
    case XKB_KEY_semicolon:
    case XKB_KEY_colon:
        return VK_OEM_1; // case ';': case ':': return 0xBA;
        // VK_OEM_PLUS (BB) Windows 2000/XP: For any country/region, the '+' key
    case XKB_KEY_plus:
    case XKB_KEY_equal:
        return VK_OEM_PLUS; // case '=': case '+': return 0xBB;
        // VK_OEM_COMMA (BC) Windows 2000/XP: For any country/region, the ',' key
    case XKB_KEY_comma:
    case XKB_KEY_less:
        return VK_OEM_COMMA; // case ',': case '<': return 0xBC;
        // VK_OEM_MINUS (BD) Windows 2000/XP: For any country/region, the '-' key
    case XKB_KEY_minus:
    case XKB_KEY_underscore:
        return VK_OEM_MINUS; // case '-': case '_': return 0xBD;
        // VK_OEM_PERIOD (BE) Windows 2000/XP: For any country/region, the '.' key
    case XKB_KEY_period:
    case XKB_KEY_greater:
        return VK_OEM_PERIOD; // case '.': case '>': return 0xBE;
        // VK_OEM_2 (BF) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key
    case XKB_KEY_slash:
    case XKB_KEY_question:
        return VK_OEM_2; // case '/': case '?': return 0xBF;
        // VK_OEM_3 (C0) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key
    case XKB_KEY_asciitilde:
    case XKB_KEY_quoteleft:
        return VK_OEM_3; // case '`': case '~': return 0xC0;
        // VK_OEM_4 (DB) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
    case XKB_KEY_bracketleft:
    case XKB_KEY_braceleft:
        return VK_OEM_4; // case '[': case '{': return 0xDB;
        // VK_OEM_5 (DC) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
    case XKB_KEY_backslash:
    case XKB_KEY_bar:
        return VK_OEM_5; // case '\\': case '|': return 0xDC;
        // VK_OEM_6 (DD) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
    case XKB_KEY_bracketright:
    case XKB_KEY_braceright:
        return VK_OEM_6; // case ']': case '}': return 0xDD;
        // VK_OEM_7 (DE) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
    case XKB_KEY_quoteright:
    case XKB_KEY_quotedbl:
        return VK_OEM_7; // case '\'': case '"': return 0xDE;
        // VK_OEM_8 (DF) Used for miscellaneous characters; it can vary by keyboard.
        // VK_OEM_102 (E2) Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
    case XKB_KEY_XF86AudioRewind:
        return 0xE3; // (E3) Android/GoogleTV: Rewind media key (Windows: VK_ICO_HELP Help key on 1984 Olivetti M24 deluxe keyboard)
    case XKB_KEY_XF86AudioForward:
        return 0xE4; // (E4) Android/GoogleTV: Fast forward media key  (Windows: VK_ICO_00 '00' key on 1984 Olivetti M24 deluxe keyboard)
        // VK_PROCESSKEY (E5) Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
        // VK_PACKET (E7) Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT,SendInput, WM_KEYDOWN, and WM_KEYUP
        // VK_ATTN (F6) Attn key
        // VK_CRSEL (F7) CrSel key
        // VK_EXSEL (F8) ExSel key
        // VK_EREOF (F9) Erase EOF key
    case XKB_KEY_XF86AudioPlay:
        return VK_PLAY; // VK_PLAY (FA) Play key
        // VK_ZOOM (FB) Zoom key
        // VK_NONAME (FC) Reserved for future use
        // VK_PA1 (FD) PA1 key
        // VK_OEM_CLEAR (FE) Clear key
    case XKB_KEY_F1:
    case XKB_KEY_F2:
    case XKB_KEY_F3:
    case XKB_KEY_F4:
    case XKB_KEY_F5:
    case XKB_KEY_F6:
    case XKB_KEY_F7:
    case XKB_KEY_F8:
    case XKB_KEY_F9:
    case XKB_KEY_F10:
    case XKB_KEY_F11:
    case XKB_KEY_F12:
    case XKB_KEY_F13:
    case XKB_KEY_F14:
    case XKB_KEY_F15:
    case XKB_KEY_F16:
    case XKB_KEY_F17:
    case XKB_KEY_F18:
    case XKB_KEY_F19:
    case XKB_KEY_F20:
    case XKB_KEY_F21:
    case XKB_KEY_F22:
    case XKB_KEY_F23:
    case XKB_KEY_F24:
        return VK_F1 + (keycode - XKB_KEY_F1);
    case XKB_KEY_VoidSymbol:
        return VK_PROCESSKEY;

    // TV keycodes from DASE / OCAP / CE-HTML standards.
    case XKB_KEY_XF86Red:
        return 0x193; // General purpose color-coded media function key, as index 0 (red)
    case XKB_KEY_XF86Green:
        return 0x194; // General purpose color-coded media function key, as index 1 (green)
    case XKB_KEY_XF86Yellow:
        return 0x195; // General purpose color-coded media function key, as index 2 (yellow)
    case XKB_KEY_XF86Blue:
        return 0x196; // General purpose color-coded media function key, as index 3 (blue)
    case XKB_KEY_XF86PowerOff:
        return 0x199; // Toggle power state
    case XKB_KEY_XF86AudioRecord:
        return 0x1A0; // Initiate or resume recording of currently selected media
    case XKB_KEY_XF86Display:
        return 0x1BC; // Swap video sources
    case XKB_KEY_XF86Subtitle:
        return 0x1CC; // Toggle display of subtitles, if available
    case XKB_KEY_XF86Video:
        return 0x26F; // Access on-demand content or programs
    default:
        break;
    }

    return 0;
}

String PlatformKeyboardEvent::singleCharacterString(unsigned val)
{
    switch (val) {
    case XKB_KEY_ISO_Enter:
    case XKB_KEY_KP_Enter:
    case XKB_KEY_Return:
        return String("\r"_s);
    case XKB_KEY_BackSpace:
        return String("\x8"_s);
    case XKB_KEY_Tab:
        return String("\t"_s);
    default:
        break;
    }

    char32_t unicodeCharacter = xkb_keysym_to_utf32(val);
    if (unicodeCharacter && U_IS_UNICODE_CHAR(unicodeCharacter))
        return makeString(unicodeCharacter);

    return { };
}
#endif // PLATFORM(CUTEFISH)

void PlatformKeyboardEvent::disambiguateKeyDownEvent(Type type, bool backwardsCompatibility)
{
    ASSERT(m_type == PlatformEvent::Type::KeyDown);
    m_type = type;
    if (backwardsCompatibility || m_handledByInputMethod)
        return;

    if (type == PlatformEvent::Type::RawKeyDown) {
        m_text = String();
        m_unmodifiedText = String();
    } else {
        m_keyIdentifier = String();
    }
}

} // namespace WebCore
