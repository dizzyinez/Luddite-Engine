#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
class World;
using SystemIDType = std::uint32_t;
struct LUDDITE_API BaseSystem
{
        virtual void Configure(World& world) {}
        virtual void Cleanup(World& world) {}
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