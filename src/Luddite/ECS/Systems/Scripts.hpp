#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_Scripts : public Luddite::System<S_Scripts>
{
	public:
	void Update(Luddite::World& world, float delta_time);
	void FixedUpdate(Luddite::World& world, float delta_time);
};
