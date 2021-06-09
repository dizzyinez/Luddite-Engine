#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Model.hpp"

struct C_Model
{
        C_Model(Luddite::BasicModelHandle ModelHandle_)
                : ModelHandle(ModelHandle_)
        {}
        Luddite::BasicModelHandle ModelHandle;
};