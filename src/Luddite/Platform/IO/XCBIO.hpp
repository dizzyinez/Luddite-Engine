#pragma once
#include "Luddite/Core/pch.hpp"

#ifdef LD_PLATFORM_LINUX
#include "xkbcommon/xkbcommon-keysyms.h"

namespace Luddite
{
enum class Keys : uint32_t
{
        A = XKB_KEY_a,
        B = XKB_KEY_b,
        C = XKB_KEY_c,
        D = XKB_KEY_d,
        E = XKB_KEY_e,
        F = XKB_KEY_f,
        G = XKB_KEY_g,
        H = XKB_KEY_h,
        I = XKB_KEY_i,
        J = XKB_KEY_j,
        K = XKB_KEY_k,
        L = XKB_KEY_l,
        M = XKB_KEY_m,
        N = XKB_KEY_n,
        O = XKB_KEY_o,
        P = XKB_KEY_p,
        Q = XKB_KEY_q,
        R = XKB_KEY_r,
        S = XKB_KEY_s,
        T = XKB_KEY_t,
        U = XKB_KEY_u,
        V = XKB_KEY_v,
        W = XKB_KEY_w,
        X = XKB_KEY_x,
        Y = XKB_KEY_y,
        Z = XKB_KEY_z,
        Zero = XKB_KEY_0,
        One = XKB_KEY_1,
        Two = XKB_KEY_2,
        Three = XKB_KEY_3,
        Four = XKB_KEY_4,
        Five = XKB_KEY_5,
        Six = XKB_KEY_6,
        Seven = XKB_KEY_7,
        Eight = XKB_KEY_8,
        Nine = XKB_KEY_9,
        Space = XKB_KEY_space,
        Exclamation = XKB_KEY_exclam,
        Quotedbl = XKB_KEY_quotedbl,
        Numbersign = XKB_KEY_numbersign,
        Dollar = XKB_KEY_dollar,
        Percent = XKB_KEY_percent,
        Ampersand = XKB_KEY_ampersand,
        Apostrophe = XKB_KEY_apostrophe,
        QuoteLeft = XKB_KEY_quoteleft,
        QuoteRight = XKB_KEY_quoteright,
        ParenthesisLeft = XKB_KEY_parenleft,
        ParenthesisRight = XKB_KEY_parenright,
        Asterisk = XKB_KEY_asterisk,
        Plus = XKB_KEY_plus,
        Comma = XKB_KEY_comma,
        Minus = XKB_KEY_minus,
        Period = XKB_KEY_period,
        Slash = XKB_KEY_slash,
        Colon = XKB_KEY_colon,
        Semicolon = XKB_KEY_semicolon,
        Less = XKB_KEY_less,
        Equal = XKB_KEY_equal,
        Greater = XKB_KEY_greater,
        QuestionMark = XKB_KEY_question,
        At = XKB_KEY_at,
        BracketLeft = XKB_KEY_bracketleft,
        BracketRight = XKB_KEY_bracketright,
        Backslash = XKB_KEY_backslash,
        Asciicircum = XKB_KEY_asciicircum,
        Underscore = XKB_KEY_underscore,
        Grave = XKB_KEY_grave,
        BraceLeft = XKB_KEY_braceleft,
        BraceRight = XKB_KEY_braceright,
        Bar = XKB_KEY_bar,
        Asciitilde = XKB_KEY_asciitilde,

        // Modifiers
        ShiftLeft = XKB_KEY_Shift_L,
        ShiftRight = XKB_KEY_Shift_R,
        ControlLeft = XKB_KEY_Control_L,
        ControlRight = XKB_KEY_Control_R,
        CapsLock = XKB_KEY_Caps_Lock,
        ShiftLock = XKB_KEY_Shift_Lock,
        MetaLeft = XKB_KEY_Meta_L,
        MetaRight = XKB_KEY_Meta_R,
        AltLeft = XKB_KEY_Alt_L,
        AltRight = XKB_KEY_Alt_R,
        SuperLeft = XKB_KEY_Super_L,
        SuperRight = XKB_KEY_Super_R,
        HyperLeft = XKB_KEY_Hyper_L,
        HyperRight = XKB_KEY_Hyper_R
};
}

#endif //LD_PLATFORM_LINUX