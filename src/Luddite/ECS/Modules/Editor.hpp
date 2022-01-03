#pragma once
#include "Luddite/Core/pch.hpp"
//#include "Luddite/ECS/Modules/LudditeBase.hpp"
//#include "Luddite/ECS/Modules/Transform3D.hpp"
//#include "Luddite/ECS/Modules/Models.hpp"
//#include "Luddite/ECS/Modules/Graphics.hpp"
#include "Luddite/ECS/Reflection.hpp"

namespace Editor
{
struct CameraParent {};
struct Components
{
        Components(flecs::world& w)
        {
                w.component<CameraParent>();
        }
};
}
