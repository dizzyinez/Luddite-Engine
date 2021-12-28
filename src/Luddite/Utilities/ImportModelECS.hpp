#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"

namespace Luddite::Utils
{
flecs::entity ImportModelECS(Handle<Model> model, flecs::world& w, flecs::entity parent);
}
