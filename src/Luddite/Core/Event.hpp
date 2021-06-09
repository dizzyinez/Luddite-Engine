#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
template <typename T>
class EventList;
class EventPool;
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
        friend class EventPool;
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
}