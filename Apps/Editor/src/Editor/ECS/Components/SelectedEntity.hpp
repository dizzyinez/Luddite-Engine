#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Entity.hpp"

struct C_SelectedEntity
{
        C_SelectedEntity() = default;
        C_SelectedEntity(Luddite::EntityID SelectedEntityID_)
                : SelectedEntityID(SelectedEntityID_)
        {
        }
        Luddite::EntityID SelectedEntityID = Luddite::NullEntityID;
};