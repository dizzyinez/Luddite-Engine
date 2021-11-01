#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_MotionStates : public Luddite::System<S_MotionStates>
{
	public:
	void FixedUpdate(Luddite::World& world, float delta_time);
};
