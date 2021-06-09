#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Entity.hpp"

struct C_ActiveCamera
{
        C_ActiveCamera(Luddite::EntityID ActiveCameraID_)
                : ActiveCameraID(ActiveCameraID_)
        {
        }
        Luddite::EntityID ActiveCameraID;
};