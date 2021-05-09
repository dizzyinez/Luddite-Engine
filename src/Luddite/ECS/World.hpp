#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "entt/entt.hpp"

namespace Luddite
{
class World
{
public:
        World();
        ~World();
private:
        entt::registry m_registry;
};
}