#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
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