#pragma once
#include "Luddite/Core/DebugTools.hpp"
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Camera.hpp"
#include "Luddite/Graphics/Color.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/ECS/Modules/Models.hpp"
#include "Luddite/ECS/Reflection.hpp"
#include "Luddite/Graphics/Lights.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "imgui.h"

namespace Graphics
{
struct MainWindow
{
        Luddite::RenderTarget RenderTarget;
};
struct RenderToMainWindow {};
struct RenderTarget
{
        Luddite::RenderTarget RenderTarget;
};

//LD_COMPONENT_DEFINE

LD_COMPONENT_DEFINE(Camera,
        (),
        ((Luddite::Camera::ProjectionType)Projection, (Luddite::Camera::ProjectionType::PERSPECTIVE)),
        ((float)FOV, (glm::radians(90.f)), .min = 20.f, .max = 179.f, .gui_elem = LD_GUI_ELEMENT_SLIDERANGLE, .format = "%.0f deg", .flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_AlwaysClamp),
        ((float)OrthoScale, (100.f)),
        ((float)ClipNear, (0.001f), .speed = 0.001f, .min = 0.001f, .max = 1.0f, .flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_AlwaysClamp),
        ((float)ClipFar, (1000.f), .min = 2.0f, .flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_AlwaysClamp)
        )
//struct Camera
//{
//        Luddite::Camera::ProjectionType Projection = Luddite::Camera::ProjectionType::PERSPECTIVE;
//        float FOV = glm::radians(90.f);
//        float OrthoScale = 100.f;
//        float ClipNear = 0.1f;
//        float ClipFar = 1000.f;
//};

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


struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                w.component<MainWindow>();
                w.set<MainWindow>({});
                w.component<RenderToMainWindow>();
                LD_COMPONENT_REGISTER(Camera, w);
                w.component<RenderTarget>();
                w.component<PointLight>();
                w.component<SpotLight>();
                w.component<DirectionalLight>();
                //w.id<MainWindow>().entity().add<RenderTarget>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.import<Transform3D::Systems>();
                w.import<Models::Components>();
                w.import<Components>();
                w.module<Systems>();

                //Rendering
                w.system<const Camera, const RenderToMainWindow>("Assign Main Window")
                .kind(w.id<Luddite::PreRender>())
                .term<RenderTarget>().inout(flecs::Out)
                .term<const MainWindow>().inout(flecs::In).singleton()
                .iter([](flecs::iter it){
                                auto rt = it.term<RenderTarget>(3);
                                auto mw = it.term<const MainWindow>(4);
                                for (auto i : it)
                                {
                                        rt[i].RenderTarget = mw[i].RenderTarget;
                                }
                        });

                w.system<>("Begin Scene")
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it){
                                Luddite::Renderer::BeginScene();
                        });

                w.system<const Transform3D::Translation, const Transform3D::Rotation, const Transform3D::Scale, const Models::Model, const Models::BoneTransforms>("Submit Meshes")
                .arg(5).oper(flecs::Optional)
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it, const Transform3D::Translation* t, const Transform3D::Rotation* r, const Transform3D::Scale* s, const Models::Model* m, const Models::BoneTransforms* bt){
                                for (auto i : it)
                                {
                                        glm::mat4 node_transforms[m[i].ModelHandle->m_Nodes.size()];
                                        int idx = 0;
                                        for (auto& node : m[i].ModelHandle->m_Nodes)
                                        {
                                                LD_VERIFY(node.m_ParentNodeID < idx, "Order of meshes in model {} are not in the right order!", m[i].ModelHandle->m_Name);
                                                if (node.m_ParentNodeID == -1)
                                                        //if (true)
                                                        node_transforms[idx] = node.m_Transform;
                                                else
                                                        //node_transforms[idx] = node.m_Transform * node_transforms[node.m_ParentNodeID];
                                                        node_transforms[idx] = node_transforms[node.m_ParentNodeID] * node.m_Transform;
                                                idx++;
                                        }
                                        for (auto& pair : m[i].ModelHandle->m_MeshNodePairs)
                                        {
                                                auto& mesh = m[i].ModelHandle->m_Meshes[pair.m_MeshIndex];
                                                Luddite::Renderer::SubmitMesh(&mesh, t[i].GetMatrix() * r[i].GetMatrix() * s[i].GetMatrix() * node_transforms[pair.m_NodeIndex], m[i].ModelHandle->m_Materials[mesh.m_MaterialIndex],
                                                        bt ? &bt[i].BoneTransforms : nullptr
                                                        );
                                        }
                                }
                        });

                w.system<const Transform3D::Translation, const PointLight>("Submit Point Lights")
                .kind(w.id<Luddite::OnRender>())
                .each([](flecs::entity e, const Transform3D::Translation& t, const PointLight& l){
                                Luddite::PointLightCPU light;
                                light.Position = glm::vec4(t.Translation, 1.f);
                                light.Color = l.Color.GetVec3();
                                light.Range = l.Range;
                                light.Intensity = l.Intensity;
                                Luddite::Renderer::SubmitPointLight(light);
                        });

                w.system<const Transform3D::Translation, const Transform3D::Rotation, const SpotLight>("Submit Spot Light")
                .kind(w.id<Luddite::OnRender>())
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

                w.system<const Transform3D::Rotation, const DirectionalLight>("Submit Directional Lights")
                .kind(w.id<Luddite::OnRender>())
                .each([](flecs::entity e, const Transform3D::Rotation& r, const DirectionalLight& l){
                                Luddite::DirectionalLightCPU light;
                                light.Color = l.Color.GetVec3();
                                light.Intensity = l.Intensity;
                                light.Direction = glm::vec4(r.GetLookDirection(), 0.0f);
                                Luddite::Renderer::SubmitDirectionalLight(light);
                        });

                w.system<>("End Scene")
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it){
                                Luddite::Renderer::EndScene();
                        });

                w.system<const Camera, const Transform3D::Translation, const Transform3D::Rotation>("Render Active Cameras")
                .term<const RenderTarget>().inout(flecs::In)
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it){
                                //.each([](flecs::entity e, const Camera& c, const Transform3D::Translation& t, const Transform3D::Rotation& r, const RenderTarget& rt){
                                auto c = it.term<const Camera>(1);
                                auto t = it.term<const Transform3D::Translation>(2);
                                auto r = it.term<const Transform3D::Rotation>(3);
                                auto rt = it.term<const RenderTarget>(4);
                                for (auto i: it)
                                {
                                        Luddite::Camera camera
                                        {
                                                t[i].Translation,
                                                r[i].GetLookDirection(),
                                                c[i].Projection,
                                                c[i].FOV,
                                                c[i].OrthoScale,
                                                c[i].ClipNear,
                                                c[i].ClipFar
                                        };
                                        Luddite::Renderer::Draw(rt[i].RenderTarget, camera);
                                }
                        });
        }
};
}
