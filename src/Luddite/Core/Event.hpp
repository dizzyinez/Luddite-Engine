#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
template <typename T>
class EventList;
class EventPool;
using EventID = uint8_t;
struct LUDDITE_API BaseEvent
{
        static inline uint8_t m_EventIDCounter;
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
        friend class EventPool;
        bool handled = false;
        static uint8_t EventID()
        {
                static uint8_t m_EventID = m_EventIDCounter++;
                return m_EventID;
        }
};


struct WindowSizeEvent : public Event<WindowSizeEvent>
{
        WindowSizeEvent (unsigned int _width, unsigned int _height) : width{_width}, height{_height} {}
        unsigned int width;
        unsigned int height;
};
}