#pragma once
#include "Luddite/Core/pch.hpp"
#include "flecs.h"
#include "flecs/addons/cpp/flecs.hpp"
namespace Luddite
{
struct InputPipeline {};
struct OnInput {};
struct PostInput {};

struct UpdatePipeline {};
struct PreUpdate {};
struct OnUpdate {};
struct PostUpdate {};

struct RenderPipeline {};
struct PreRender {};
struct OnRender {};
struct PostRender {};

struct SimulationPipeline {};
struct PreSimulate {};
struct OnSimulate {};
struct PostSimulate {};

struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                //w.component<InputPipeline>();
                auto on_input = w.component<OnInput           >();
                auto post_input = w.component<PostInput       >();

                //w.component<UpdatePipeline>();
                auto pre_update = w.component<PreUpdate       >();
                auto on_update = w.component<OnUpdate         >();
                auto post_update = w.component<PostUpdate     >();

                //w.component<RenderPipeline>();
                auto pre_render = w.component<PreRender       >();
                auto on_render = w.component<OnRender         >();
                auto post_render = w.component<PostRender     >();

                //w.component<SimulationPipeline                >();
                auto pre_simulate = w.component<PreSimulate   >();
                auto on_simulate = w.component<OnSimulate     >();
                auto post_simulate = w.component<PostSimulate >();

                w.component<InputPipeline>("InputPipeline")
                .add(flecs::Pipeline);
                w.pipeline("Input Pipeline")
                .add(on_input)
                .add(post_input);

                w.component<UpdatePipeline>("UpdatePipeline")
                .add(flecs::Pipeline)
                .add(pre_update)
                .add(on_update)
                .add(post_update);

                w.component<RenderPipeline>("RenderPipeline");
                auto rp = w.pipeline("Render Pipeline")
                          .add(pre_render)
                          .add(on_render)
                          .add(post_render);

                w.component<SimulationPipeline>("SimulationPipeline")
                .add(flecs::Pipeline)
                .add(pre_simulate)
                .add(on_simulate)
                .add(post_simulate);

                w.set_pipeline(rp);
                //ecs_set_pipeline(w.c_ptr(), rp.id());

                //w.system<>().kind(w.id<OnLoad>());
        }
};
}
