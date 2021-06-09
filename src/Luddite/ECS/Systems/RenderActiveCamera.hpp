#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_RenderActiveCamera : public Luddite::System<S_RenderActiveCamera>
{
        public:
        void configure()
        {
                // world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>);
        }

        void Update(Luddite::World& world, Luddite::RenderTarget render_target)
        {
                auto active_camera = world.GetEntityFromID(world.GetSingleton<C_ActiveCamera>().ActiveCameraID);
                auto& c_transform = active_camera.GetComponent<C_Transform3D>();
                auto& c_camera = active_camera.GetComponent<C_Camera>();
                Luddite::Camera camera
                {
                        c_transform.Translation,
                        c_transform.GetLookDirection(),
                        c_camera.Projection,
                        c_camera.FOV,
                        c_camera.OrthoScale,
                        c_camera.ClipNear,
                        c_camera.ClipFar
                };
                Luddite::Renderer::Draw(render_target, camera);
        }
};