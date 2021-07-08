#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"

struct C_Model
{
        C_Model(Luddite::BasicModel::Handle ModelHandle_)
                : ModelHandle(ModelHandle_)
        {}
        Luddite::BasicModel::Handle ModelHandle;
};