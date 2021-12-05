#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Reflection.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"

namespace Transform3D
{
LD_COMPONENT_DEFINE(Translation,
        (
                inline glm::mat4 GetMatrix() const {return glm::translate(glm::mat4(1.f), Translation);}
        ),
        ((glm::vec3)Translation, ({0.f, 0.f, 0.f}))
        )


LD_COMPONENT_DEFINE(Rotation,
        (
                inline glm::mat4 GetMatrix() const {return glm::toMat4(glm::quat(Rotation));}
                inline glm::vec3 GetLookDirection() const {return glm::toMat4(glm::quat(Rotation)) * glm::vec4(0.f, 0.f, 1.f, 0.f);}
        ),
        ((glm::vec3)Rotation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(Scale,
        (
                inline glm::mat4 GetMatrix() const {return glm::scale(glm::mat4(1.f), Scale);}
        ),
        ((glm::vec3)Scale, ({1.f, 1.f, 1.f}))
        )

struct Components
{
        Components(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.module<Components>();
                LD_COMPONENT_REGISTER(Translation, "Translation", w);
                LD_COMPONENT_REGISTER(Scale, "Scale", w);
                LD_COMPONENT_REGISTER(Rotation, "Rotation", w);

                //w.id<Luddite::Scene>().entity().add<Translation>();
                //w.id<Luddite::Scene>().entity().add<Rotation>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Components>();
                w.module<Systems>();
        }
};
}
