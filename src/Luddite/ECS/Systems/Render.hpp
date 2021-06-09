#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_Render : public Luddite::System<S_Render>
{
        public:
        void configure()
        {
                // world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>);
        }

        void Update(Luddite::World& world, float alpha, Luddite::RenderTarget render_target)
        {
                Luddite::Renderer::BeginScene();
                for (const auto &&[id, transform, model]: world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>).each())
                {
                        for (auto& mesh : model.ModelHandle->meshes)
                                Luddite::Renderer::SubmitMesh(mesh, transform.GetTransformMatrix());
                }
                auto active_camera = world.GetEntityFromID(world.GetSingleton<C_ActiveCamera>().ActiveCameraID);
                auto& c_transform = active_camera.GetComponent<C_Transform3D>();
                auto& c_camera = active_camera.GetComponent<C_Camera>();
                LD_LOG_TRACE("{}", glm::to_string(c_transform.Translation));
                glm::vec3 view_direction = glm::normalize(-c_transform.Translation);
                Luddite::Renderer::EndScene();
                Luddite::Camera camera
                {
                        c_transform.Translation,
                        view_direction,
                        c_camera.Projection,
                        c_camera.FOV,
                        c_camera.OrthoScale,
                        c_camera.ClipNear,
                        c_camera.ClipFar
                };
                // camera.Position = glm::vec3(0.f, 0.f, -5.f);
                // camera.ViewDirection = glm::vec3(0.f, 0.f, 1.f);
                Luddite::Renderer::Draw(render_target, camera);
        }
};