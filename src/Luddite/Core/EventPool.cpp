#include "Luddite/Core/EventPool.hpp"

namespace Luddite
{
std::unordered_map<EventIDType, EventList<BaseEvent>* > Events::m_EventMap;
}
