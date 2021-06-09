#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Group.hpp"

namespace Luddite
{
using SystemIDType = std::uint32_t;
struct LUDDITE_API BaseSystem
{
        virtual void Configure() {}
        static inline SystemIDType m_SystemIDCounter;
};

template <typename T>
class LUDDITE_API System : public BaseSystem
{
public:
        virtual ~System()
        {
        }
private:
        friend class World;
        static SystemIDType SystemID()
        {
                static SystemIDType m_SystemID = m_SystemIDCounter++;
                return m_SystemID;
        }
};
}