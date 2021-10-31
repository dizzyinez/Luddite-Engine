#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"

struct C_Model
{
        C_Model(Luddite::Handle<Luddite::Model> ModelHandle_)
                : ModelHandle(ModelHandle_)
        {}
        Luddite::Handle<Luddite::Model> ModelHandle;
};