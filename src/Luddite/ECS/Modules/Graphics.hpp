#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Camera.hpp"
#include "Luddite/Graphics/Color.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/Graphics/Lights.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Graphics/Renderer.hpp"

namespace Graphics
{
struct RenderTarget
{
        Luddite::RenderTarget RenderTarget;
};

struct Camera
{
        Luddite::Camera::ProjectionType Projection = Luddite::Camera::ProjectionType::PERSPECTIVE;
        float FOV = glm::radians(90.f);
        float OrthoScale = 100.f;
        float ClipNear = 0.1f;
        float ClipFar = 1000.f;
};

struct PointLight
{
        Luddite::ColorRGB Color;
        float Range = 10.f;
        float Intensity = 1.f;
};

struct SpotLight
{
        Luddite::ColorRGB Color;
        float Range = 10.f;
        glm::vec3 Direction;
        float Intensity = 1.f;
        float HalfAngle = glm::quarter_pi<float>();
};

struct DirectionalLight
{
        Luddite::ColorRGB Color;
        float Intensity = 1.f;
};

struct Model
{
        Luddite::Handle<Luddite::Model> ModelHandle;
};

struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                w.component<Camera>("Camera");
                w.component<PointLight>("Point Light");
                w.component<SpotLight>("Spot Light");
                w.component<DirectionalLight>("Directional Light");
                w.component<Model>("Model");
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Transform3D::Components>();
                w.import<Components>();
                w.module<Systems>();
                w.system<const RenderTarget, const Camera, const Transform3D::Translation, const Transform3D::Rotation>("Begin Scene")
                .iter([](flecs::iter it){
                                Luddite::Renderer::BeginScene();
                        });
                w.system<const Transform3D::Translation, const Transform3D::Rotation, const Transform3D::Scale, const Model>()
                .each([](flecs::entity e, const Transform3D::Translation& t, const Transform3D::Rotation& r, const Transform3D::Scale& s, const Model& m){
                                glm::mat4 node_transforms[m.ModelHandle->m_Nodes.size()];
                                int idx = 0;
                                for (auto& node : m.ModelHandle->m_Nodes)
                                {
                                        if (node.m_ParentNodeID >= 0)
                                                node_transforms[idx] = node.m_Transform * node_transforms[node.m_ParentNodeID];
                                        else
                                                node_transforms[idx] = node.m_Transform;
                                }
                                for (auto& pair : m.ModelHandle->m_MeshNodePairs)
                                {
                                        auto& mesh = m.ModelHandle->m_Meshes[pair.m_MeshIndex];
                                        Luddite::Renderer::SubmitMesh(&mesh, t.GetMatrix() * r.GetMatrix() * s.GetMatrix() * node_transforms[pair.m_NodeIndex], m.ModelHandle->m_Materials[mesh.m_MaterialIndex]);
                                }
                        });
                w.system<const Transform3D::Translation, const PointLight>()
                .each([](flecs::entity e, const Transform3D::Translation& t, const PointLight& l){
                                Luddite::PointLightCPU light;
                                light.Position = glm::vec4(t.Translation, 1.f);
                                light.Color = l.Color.GetVec3();
                                light.Range = l.Range;
                                light.Intensity = l.Intensity;
                                Luddite::Renderer::SubmitPointLight(light);
                        });
                w.system<const Transform3D::Translation, const Transform3D::Rotation, const SpotLight>()
                .each([](flecs::entity e, const Transform3D::Translation& t, const Transform3D::Rotation& r, const SpotLight& l){
                                Luddite::SpotLightCPU light;
                                light.Position = glm::vec4(t.Translation, 1.f);
                                light.Color = l.Color.GetVec3();
                                light.Range = l.Range;
                                light.Intensity = l.Intensity;
                                light.HalfAngle = l.HalfAngle;
                                light.Direction = glm::vec4(r.GetLookDirection(), 0.0f);
                                Luddite::Renderer::SubmitSpotLight(light);
                        });
                w.system<const Transform3D::Rotation, const DirectionalLight>()
                .each([](flecs::entity e, const Transform3D::Rotation& r, const DirectionalLight& l){
                                Luddite::DirectionalLightCPU light;
                                light.Color = l.Color.GetVec3();
                                light.Intensity = l.Intensity;
                                light.Direction = glm::vec4(r.GetLookDirection(), 0.0f);
                                Luddite::Renderer::SubmitDirectionalLight(light);
                        });
                w.system<const RenderTarget, const Camera, const Transform3D::Translation, const Transform3D::Rotation>("End Scene")
                .iter([](flecs::iter it){
                                Luddite::Renderer::BeginScene();
                        });
                w.system<const RenderTarget, const Camera, const Transform3D::Translation, const Transform3D::Rotation>("Render Active Cameras")
                .each([](flecs::entity e, const RenderTarget& rt, const Camera& c, const Transform3D::Translation& t, const Transform3D::Rotation& r){
                                Luddite::Camera camera
                                {
                                        t.Translation,
                                        r.GetLookDirection(),
                                        c.Projection,
                                        c.FOV,
                                        c.OrthoScale,
                                        c.ClipNear,
                                        c.ClipFar
                                };
                                Luddite::Renderer::Draw(rt.RenderTarget, camera);
                        });
        }
};
}
