#include "Luddite/ECS/Systems/MotionStates.hpp"
#include "Luddite/Core/Profiler.hpp"
#include "Luddite/ECS/Entity.hpp"
#include "Luddite/ECS/Components/Components.hpp"
#include "Luddite/ECS/World.hpp"

void S_MotionStates::FixedUpdate(Luddite::World &world, float delta_time)
{
	LD_PROFILE_FUNCTION();
	for (auto && [entity, c_motion_state, c_transform] : world.GetGroup<C_MotionState>(Luddite::Borrow<C_Transform3D>).each())
	{
		c_motion_state.TransformsRingBuffer.push(c_transform);
	}
}
