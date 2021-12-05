#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Platform/IO/IO.hpp"

namespace Luddite
{
template <typename T>
class EventList;
class Events;
using EventIDType = uint8_t;
struct LUDDITE_API BaseEvent
{
        static inline EventIDType m_EventIDCounter;
};

template <typename T>
struct LUDDITE_API Event : public BaseEvent
{
        public:
        virtual ~Event()
        {
        }
        inline void SetHandled() {handled = true;}
        inline void SetUnhandled() {handled = false;}
        private:
        template <typename> friend class EventList;
        friend class Events;
        static EventIDType EventID()
        {
                static EventIDType m_EventID = m_EventIDCounter++;
                return m_EventID;
        }

        bool handled = false;
};


struct WindowSizeEvent : public Event<WindowSizeEvent>
{
        WindowSizeEvent (unsigned int _width, unsigned int _height) : width{_width}, height{_height} {}
        unsigned int width;
        unsigned int height;
};

struct KeyPressEvent : public Event<KeyPressEvent>
{
        KeyPressEvent(uint32_t key_code_) : key_code(static_cast<Luddite::Keys>(key_code_)) {}
        KeyPressEvent(Luddite::Keys key_code_) : key_code{key_code_} {}
        Luddite::Keys key_code;
};

struct KeyReleaseEvent : public Event<KeyReleaseEvent>
{
        KeyReleaseEvent(uint32_t key_code_) : key_code(static_cast<Luddite::Keys>(key_code_)) {}
        KeyReleaseEvent(Luddite::Keys key_code_) : key_code{key_code_} {}
        Luddite::Keys key_code;
};

struct MouseButtonPressEvent : public Event<MouseButtonPressEvent>
{
        MouseButtonPressEvent(uint8_t button_) : button{button_} {}
        uint8_t button;
};

struct MouseButtonReleaseEvent : public Event<MouseButtonReleaseEvent>
{
        MouseButtonReleaseEvent(uint8_t button_) : button{button_} {}
        uint8_t button;
};

struct MouseScrollEvent : public Event<MouseScrollEvent>
{
        MouseScrollEvent(int16_t scrolls_) : scrolls{scrolls_} {}
        int16_t scrolls;
};

struct MouseMotionEvent : public Event<MouseMotionEvent>
{
        MouseMotionEvent(int16_t x_, int16_t y_, uint16_t button_mask_) : x{x_}, y{y_}, button_mask{button_mask_} {}
        int16_t x;
        int16_t y;
        uint16_t button_mask;
};
}
