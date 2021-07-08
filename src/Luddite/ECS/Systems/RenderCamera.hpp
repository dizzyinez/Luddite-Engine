#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_RenderCamera : public Luddite::System<S_RenderCamera>
{
        public:

        void Update(Luddite::World& world, Luddite::RenderTarget render_target, Luddite::EntityID cameraID)
        {
                auto active_camera = world.GetEntityFromID(cameraID);
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