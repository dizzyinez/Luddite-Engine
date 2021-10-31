#include "Luddite/ECS/Systems/Scripts.hpp"
#include "Luddite/ECS/Components/NativeScript.hpp"
#include "Luddite/Core/Profiler.hpp"
#include "Luddite/ECS/World.hpp"

void S_Scripts::Update(Luddite::World &world, float delta_time)
{
	LD_PROFILE_FUNCTION();
	for (auto && [entity, c_script] : world.GetView<C_NativeScript>().each())
	{
		if (!c_script.m_pScript)
			c_script.m_pScript = c_script.InstantiateScript();
		c_script.m_pScript->OnInput();
		c_script.m_pScript->OnUpdate(delta_time, world, world.GetEntityFromID(entity));
	}
}

void S_Scripts::FixedUpdate(Luddite::World &world, float delta_time)
{
	LD_PROFILE_FUNCTION();
	for (auto && [entity, c_script] : world.GetView<C_NativeScript>().each())
	{
		if (!c_script.m_pScript)
			c_script.m_pScript = c_script.InstantiateScript();
		c_script.m_pScript->OnFixedUpdate(delta_time, world, world.GetEntityFromID(entity));
	}
};
