#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Platform/IO/NativeIO.hpp"

namespace Luddite
{
enum class ModifierBits : uint16_t
{
        NONE = 0,
        ShiftLeft = 1 << 0,
        ShiftRight = 1 << 1,
        ControlLeft = 1 << 2,
        ControlRight = 1 << 3,
        CapsLock = 1 << 4,
        ShiftLock = 1 << 5,
        MetaLeft = 1 << 6,
        MetaRight = 1 << 7,
        AltLeft = 1 << 8,
        AltRight = 1 << 9,
        SuperLeft = 1 << 10,
        SuperRight = 1 << 11,
        HyperLeft = 1 << 12,
        HyperRight = 1 << 13,
};
namespace IO
{
static constexpr int GetKeyCode(Keys key)
{
        switch (key)
        {
        case Keys::SPACE: return 0; break;

        case Keys::APOSTROPHE: return 1; break;

        case Keys::COMMA: return 2; break;

        case Keys::MINUS: return 3; break;

        case Keys::PERIOD: return 4; break;

        case Keys::SLASH: return 5; break;

        case Keys::ZERO: return 6; break;

        case Keys::ONE: return 7; break;

        case Keys::TWO: return 8; break;

        case Keys::THREE: return 9; break;

        case Keys::FOUR: return 10; break;

        case Keys::FIVE: return 11; break;

        case Keys::SIX: return 12; break;

        case Keys::SEVEN: return 13; break;

        case Keys::EIGHT: return 14; break;

        case Keys::NINE: return 15; break;

        case Keys::SEMICOLON: return 16; break;

        case Keys::EQUAL: return 17; break;

        case Keys::A: return 18; break;

        case Keys::B: return 19; break;

        case Keys::C: return 20; break;

        case Keys::D: return 21; break;

        case Keys::E: return 22; break;

        case Keys::F: return 23; break;

        case Keys::G: return 24; break;

        case Keys::H: return 25; break;

        case Keys::I: return 26; break;

        case Keys::J: return 27; break;

        case Keys::K: return 28; break;

        case Keys::L: return 29; break;

        case Keys::M: return 30; break;

        case Keys::N: return 31; break;

        case Keys::O: return 32; break;

        case Keys::P: return 33; break;

        case Keys::Q: return 34; break;

        case Keys::R: return 35; break;

        case Keys::S: return 36; break;

        case Keys::T: return 37; break;

        case Keys::U: return 38; break;

        case Keys::V: return 39; break;

        case Keys::W: return 40; break;

        case Keys::X: return 41; break;

        case Keys::Y: return 42; break;

        case Keys::Z: return 43; break;

        case Keys::LEFT_BRACKET: return 44; break;

        case Keys::BACKSLASH: return 45; break;

        case Keys::RIGHT_BRACKET: return 46; break;

        case Keys::GRAVE_ACCENT: return 47; break;

        case Keys::WORLD_1: return 48; break;

        case Keys::WORLD_2: return 49; break;

        case Keys::ESCAPE: return 50; break;

        case Keys::ENTER: return 51; break;

        case Keys::TAB: return 52; break;

        case Keys::BACKSPACE: return 53; break;

        case Keys::INSERT: return 54; break;

        case Keys::DELETE: return 55; break;

        case Keys::RIGHT: return 56; break;

        case Keys::LEFT: return 57; break;

        case Keys::DOWN: return 58; break;

        case Keys::UP: return 59; break;

        case Keys::PAGE_UP: return 60; break;

        case Keys::PAGE_DOWN: return 61; break;

        case Keys::HOME: return 62; break;

        case Keys::END: return 63; break;

        case Keys::CAPS_LOCK: return 64; break;

        case Keys::SCROLL_LOCK: return 65; break;

        case Keys::NUM_LOCK: return 66; break;

        case Keys::PRINT_SCREEN: return 67; break;

        case Keys::PAUSE: return 68; break;

        case Keys::F1: return 69; break;

        case Keys::F2: return 70; break;

        case Keys::F3: return 71; break;

        case Keys::F4: return 72; break;

        case Keys::F5: return 73; break;

        case Keys::F6: return 74; break;

        case Keys::F7: return 75; break;

        case Keys::F8: return 76; break;

        case Keys::F9: return 77; break;

        case Keys::F10: return 78; break;

        case Keys::F11: return 79; break;

        case Keys::F12: return 80; break;

        case Keys::F13: return 81; break;

        case Keys::F14: return 82; break;

        case Keys::F15: return 83; break;

        case Keys::F16: return 84; break;

        case Keys::F17: return 85; break;

        case Keys::F18: return 86; break;

        case Keys::F19: return 87; break;

        case Keys::F20: return 88; break;

        case Keys::F21: return 89; break;

        case Keys::F22: return 90; break;

        case Keys::F23: return 91; break;

        case Keys::F24: return 92; break;

        case Keys::F25: return 93; break;

        case Keys::KP_0: return 94; break;

        case Keys::KP_1: return 95; break;

        case Keys::KP_2: return 96; break;

        case Keys::KP_3: return 97; break;

        case Keys::KP_4: return 98; break;

        case Keys::KP_5: return 99; break;

        case Keys::KP_6: return 100; break;

        case Keys::KP_7: return 101; break;

        case Keys::KP_8: return 102; break;

        case Keys::KP_9: return 103; break;

        case Keys::KP_DECIMAL: return 104; break;

        case Keys::KP_DIVIDE: return 105; break;

        case Keys::KP_MULTIPLY: return 106; break;

        case Keys::KP_SUBTRACT: return 107; break;

        case Keys::KP_ADD: return 108; break;

        case Keys::KP_ENTER: return 109; break;

        case Keys::KP_EQUAL: return 110; break;

        case Keys::LEFT_SHIFT: return 111; break;

        case Keys::LEFT_CONTROL: return 112; break;

        case Keys::LEFT_ALT: return 113; break;

        case Keys::LEFT_SUPER: return 114; break;

        case Keys::RIGHT_SHIFT: return 115; break;

        case Keys::RIGHT_CONTROL: return 116; break;

        case Keys::RIGHT_ALT: return 117; break;

        case Keys::RIGHT_SUPER: return 118; break;

        case Keys::MENU: return 119; break;

        case Keys::UNKNOWN: return 120; break;
        }
};
static inline constexpr uint32_t GetNumKeys() {return 121;}
static constexpr uint16_t GetModBit(Keys key)
{
        switch (key)
        {
        case Keys::LEFT_SHIFT: return static_cast<uint16_t>(ModifierBits::ShiftLeft);

        case Keys::RIGHT_SHIFT: return static_cast<uint16_t>(ModifierBits::ShiftRight);

        case Keys::LEFT_CONTROL: return static_cast<uint16_t>(ModifierBits::ControlLeft);

        case Keys::RIGHT_CONTROL: return static_cast<uint16_t>(ModifierBits::ControlRight);

        case Keys::CAPS_LOCK: return static_cast<uint16_t>(ModifierBits::CapsLock);

        //case Keys::SHIFT_LOCK: return static_cast<uint16_t>(ModifierBits::ShiftLock);

        //case Keys::MetaLeft: return static_cast<uint16_t>(ModifierBits::MetaLeft);

        //case Keys::MetaRight: return static_cast<uint16_t>(ModifierBits::MetaRight);

        case Keys::LEFT_ALT: return static_cast<uint16_t>(ModifierBits::AltLeft);

        case Keys::RIGHT_ALT: return static_cast<uint16_t>(ModifierBits::AltRight);

        case Keys::LEFT_SUPER: return static_cast<uint16_t>(ModifierBits::SuperLeft);

        case Keys::RIGHT_SUPER: return static_cast<uint16_t>(ModifierBits::SuperRight);

        //case Keys::HyperLeft: return static_cast<uint16_t>(ModifierBits::HyperLeft);

        //case Keys::HyperRight: return static_cast<uint16_t>(ModifierBits::HyperRight);

        default: return 0;
        }
};
};
}
