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
struct IO
{
        struct Keyboard
        {
        };
        struct Mouse
        {
        };
        static constexpr uint32_t GetKeyCode(Keys key)
        {
                switch (key)
                {
                case Keys::A: return 0;

                case Keys::B: return 1;

                case Keys::C: return 2;

                case Keys::D: return 3;

                case Keys::E: return 4;

                case Keys::F: return 5;

                case Keys::G: return 6;

                case Keys::H: return 7;

                case Keys::I: return 8;

                case Keys::J: return 9;

                case Keys::K: return 10;

                case Keys::L: return 11;

                case Keys::M: return 12;

                case Keys::N: return 13;

                case Keys::O: return 14;

                case Keys::P: return 15;

                case Keys::Q: return 16;

                case Keys::R: return 17;

                case Keys::S: return 18;

                case Keys::T: return 19;

                case Keys::U: return 20;

                case Keys::V: return 21;

                case Keys::W: return 22;

                case Keys::X: return 23;

                case Keys::Y: return 24;

                case Keys::Z: return 25;

                case Keys::Zero: return 26;

                case Keys::One: return 27;

                case Keys::Two: return 28;

                case Keys::Three: return 29;

                case Keys::Four: return 30;

                case Keys::Five: return 31;

                case Keys::Six: return 32;

                case Keys::Seven: return 33;

                case Keys::Eight: return 34;

                case Keys::Nine: return 35;

                case Keys::Space: return 36;

                case Keys::Exclamation: return 37;

                case Keys::Quotedbl: return 38;

                case Keys::Numbersign: return 39;

                case Keys::Dollar: return 40;

                case Keys::Percent: return 41;

                case Keys::Ampersand: return 42;

                case Keys::Apostrophe: return 43;

                case Keys::ParenthesisLeft: return 44;

                case Keys::ParenthesisRight: return 45;

                case Keys::Asterisk: return 46;

                case Keys::Plus: return 47;

                case Keys::Comma: return 48;

                case Keys::Minus: return 49;

                case Keys::Period: return 50;

                case Keys::Slash: return 51;

                case Keys::Colon: return 52;

                case Keys::Semicolon: return 53;

                case Keys::Less: return 54;

                case Keys::Equal: return 55;

                case Keys::Greater: return 56;

                case Keys::QuestionMark: return 57;

                case Keys::At: return 58;

                case Keys::BracketLeft: return 59;

                case Keys::BracketRight: return 60;

                case Keys::Backslash: return 61;

                case Keys::Asciicircum: return 62;

                case Keys::Underscore: return 63;

                case Keys::Grave: return 64;

                case Keys::BraceLeft: return 65;

                case Keys::BraceRight: return 66;

                case Keys::Bar: return 67;

                case Keys::Asciitilde: return 68;

                case Keys::ShiftLeft: return 69;

                case Keys::ShiftRight: return 70;

                case Keys::ControlLeft: return 71;

                case Keys::ControlRight: return 72;

                case Keys::CapsLock: return 73;

                case Keys::ShiftLock: return 74;

                case Keys::MetaLeft: return 75;

                case Keys::MetaRight: return 76;

                case Keys::AltLeft: return 77;

                case Keys::AltRight: return 78;

                case Keys::SuperLeft: return 79;

                case Keys::SuperRight: return 80;

                case Keys::HyperLeft: return 81;

                case Keys::HyperRight: return 82;

                case Keys::F1: return 83;

                case Keys::F2: return 84;

                case Keys::F3: return 85;

                case Keys::F4: return 86;

                case Keys::F5: return 87;

                case Keys::F6: return 88;

                case Keys::F7: return 89;

                case Keys::F8: return 90;

                case Keys::F9: return 91;

                case Keys::F10: return 92;

                case Keys::F11: return 93;

                case Keys::F12: return 94;

                case Keys::F13: return 95;

                case Keys::F14: return 96;

                case Keys::F15: return 97;

                case Keys::F16: return 98;

                case Keys::F17: return 99;

                case Keys::F18: return 100;

                case Keys::F19: return 101;

                case Keys::F20: return 102;

                case Keys::F21: return 103;

                case Keys::F22: return 104;

                case Keys::F23: return 105;

                case Keys::F24: return 106;

                case Keys::F25: return 107;

                case Keys::F26: return 108;

                case Keys::F27: return 109;

                case Keys::F28: return 110;

                case Keys::F29: return 111;

                case Keys::F30: return 112;

                case Keys::F31: return 113;

                case Keys::F32: return 114;

                case Keys::F33: return 115;

                case Keys::F34: return 116;

                case Keys::F35: return 117;
                }
        };
        static constexpr uint16_t GetModBit(Keys key)
        {
                switch (key)
                {
                case Keys::ShiftLeft: return static_cast<uint16_t>(ModifierBits::ShiftLeft);

                case Keys::ShiftRight: return static_cast<uint16_t>(ModifierBits::ShiftRight);

                case Keys::ControlLeft: return static_cast<uint16_t>(ModifierBits::ControlLeft);

                case Keys::ControlRight: return static_cast<uint16_t>(ModifierBits::ControlRight);

                case Keys::CapsLock: return static_cast<uint16_t>(ModifierBits::CapsLock);

                case Keys::ShiftLock: return static_cast<uint16_t>(ModifierBits::ShiftLock);

                case Keys::MetaLeft: return static_cast<uint16_t>(ModifierBits::MetaLeft);

                case Keys::MetaRight: return static_cast<uint16_t>(ModifierBits::MetaRight);

                case Keys::AltLeft: return static_cast<uint16_t>(ModifierBits::AltLeft);

                case Keys::AltRight: return static_cast<uint16_t>(ModifierBits::AltRight);

                case Keys::SuperLeft: return static_cast<uint16_t>(ModifierBits::SuperLeft);

                case Keys::SuperRight: return static_cast<uint16_t>(ModifierBits::SuperRight);

                case Keys::HyperLeft: return static_cast<uint16_t>(ModifierBits::HyperLeft);

                case Keys::HyperRight: return static_cast<uint16_t>(ModifierBits::HyperRight);

                default: return 0;
                }
        };
};
}