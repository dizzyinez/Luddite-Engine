#pragma once
#include "Luddite/Core/DebugTools.hpp"
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Camera.hpp"
#include "Luddite/Graphics/Color.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/ECS/Reflection.hpp"
#include "Luddite/Graphics/Lights.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "flecs/addons/cpp/flecs.hpp"
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

struct Model
{
        Luddite::Handle<Luddite::Model> ModelHandle;
};

struct BoneTransforms
{
        std::vector<glm::mat4> BoneTransforms;
};

struct NodeTransforms
{
        std::vector<glm::mat4> NodeTransforms; //Within frame, initially set as local, then replaced with model space
};

#ifndef LD_MAX_ANIMATIONS
#define LD_MAX_ANIMATIONS 4
#endif
struct AnimationStack
{
        struct AnimationData
        {
                int Index = -1;
                int PlayCount = -1;
                double Timer = 0.;
                float Weight = 1.;
        };
        std::array<AnimationData, LD_MAX_ANIMATIONS> Animations{};
        void PlayAnimation(int Index, int PlayCount = 1, double StartTime = 0)
        {
                for (int i = 0; i < LD_MAX_ANIMATIONS; i++)
                {
                        if (Animations[i].Index == -1)
                        {
                                Animations[i] = {Index, PlayCount, StartTime};
                                return;
                        }
                }
                LD_LOG_WARN("Tried to play animation (index {}), but there are already {}/{} animations playing currently! Increase by defining LD_MAX_ANIMATIONS", Index, LD_MAX_ANIMATIONS, LD_MAX_ANIMATIONS);
        }
        void StopAnimation(int Index)
        {
                for (int i = 0; i < LD_MAX_ANIMATIONS; i++)
                {
                        if (Animations[i].Index == Index)
                                Animations[i].Index = -1;
                }
        }
        void StopAllAnimations()
        {
                for (int i = 0; i < LD_MAX_ANIMATIONS; i++)
                        Animations[i].Index = -1;
        }
        AnimationData& slot(int idx) {return Animations.at(idx);}
        const AnimationData& slot(int idx) const {return Animations.at(idx);}
};

struct ModelNodeID
{
        unsigned int NodeID;
};

//struct ModelBoneID
//{
//        unsigned int BoneID;
//};

struct CopyNodeModelTransformFromParent {};
struct CopyNodeModelTransformToParent {};
struct CopyNodeLocalTransformFromParent {};
struct CopyNodeLocalTransformToParent {};

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
                w.component<Model>();
                w.component<BoneTransforms>();
                w.component<NodeTransforms>();
                w.component<AnimationStack>();
                w.component<ModelNodeID>();
                //w.component<ModelBoneID>();
                w.component<CopyNodeModelTransformFromParent>();
                w.component<CopyNodeModelTransformToParent>();
                w.component<CopyNodeLocalTransformFromParent>();
                w.component<CopyNodeLocalTransformToParent>();
                //w.id<MainWindow>().entity().add<RenderTarget>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.import<Transform3D::Systems>();
                w.import<Components>();
                w.module<Systems>();

                //Animations
                w.system<NodeTransforms, const Model, AnimationStack>("Progress Animations")
                .kind(w.id<Luddite::OnSimulate>())
                //.iter([](flecs::iter it, RootNode* root, const Model* m, AnimationStack* as){
                .iter([](flecs::iter it){
                                auto nt = it.term<NodeTransforms>(1);
                                auto m = it.term<const Model>(2);
                                auto as = it.term<AnimationStack>(3);
                                for (auto i : it)
                                {
                                        struct NodeData
                                        {
                                                glm::vec3 pos;
                                                glm::quat rot;
                                                glm::vec3 scale;
                                                bool dirty = false;
                                                //bool dirty_pos = false;
                                                //bool dirty_rot = false;
                                                //bool dirty_scale = false;
                                                float total_weight = 0.;
                                        };
                                        //TODO: replace w/ c style array
                                        auto num_nodes = m[i].ModelHandle->m_Nodes.size();
                                        NodeData node_data[num_nodes];

                                        //for (const auto& node : m[i].ModelHandle->m_Nodes)
                                        for (unsigned int n = 0; n < m[i].ModelHandle->m_Nodes.size(); n++)
                                        {
                                                const auto& node = m[i].ModelHandle->m_Nodes[n];
                                                node_data[n] = NodeData{node.m_Pos, glm::quat(node.m_Rot), node.m_Scale};
                                        }
                                        for (auto& anim : as[i].Animations)
                                        {
                                                if (anim.Index != -1)
                                                {
                                                        //LD_LOG_INFO("Playing animation, name: {}, idx: {}, weight: {}, timer: {}", m[i].ModelHandle->m_Animations.at(anim.Index).m_Name, anim.Index, anim.Weight, anim.Timer);
                                                        const auto& animation_data = m[i].ModelHandle->m_Animations.at(anim.Index);
                                                        anim.Timer += it.delta_time() * animation_data.m_TicksPerSecond;
                                                        while (anim.Timer > animation_data.m_Duration)
                                                        {
                                                                anim.Timer -= animation_data.m_Duration;
                                                                if (anim.PlayCount > 0)
                                                                        anim.PlayCount--;
                                                        }

                                                        if (anim.PlayCount == 0)
                                                        {
                                                                anim.Index = -1;
                                                                continue;
                                                        }

                                                        for (const auto& channel : animation_data.m_Channels)
                                                        {
                                                                auto& channel_node_data = node_data[channel.m_NodeID];
                                                                if (!channel_node_data.dirty)
                                                                {
                                                                        channel_node_data.dirty = true;
                                                                        channel_node_data.pos = glm::vec3{0.};
                                                                        channel_node_data.scale = glm::vec3{1.};
                                                                        channel_node_data.rot = glm::identity<glm::quat>();
                                                                }

                                                                channel_node_data.pos += anim.Weight * channel.GetInterpolatedPosition(anim.Timer);
                                                                //https://gamedev.stackexchange.com/questions/62354/method-for-interpolation-between-3-quaternions
                                                                channel_node_data.rot = glm::slerp(
                                                                        channel_node_data.rot,
                                                                        channel.GetInterpolatedRotation(anim.Timer),
                                                                        anim.Weight / (channel_node_data.total_weight + anim.Weight)
                                                                        );
                                                                channel_node_data.scale *= glm::pow(channel.GetInterpolatedScale(anim.Timer), glm::vec3{anim.Weight});
                                                                node_data[channel.m_NodeID].total_weight += anim.Weight;
                                                        }
                                                }
                                        }

                                        nt[i].NodeTransforms.resize(num_nodes);
                                        for (auto n = 0; n < nt[i].NodeTransforms.size(); n++)
                                        {
                                                nt[i].NodeTransforms[n] = glm::scale(glm::translate(glm::identity<glm::mat4>(), node_data[n].pos) * glm::toMat4(node_data[n].rot),
                                                        node_data[n].scale
                                                        );
                                        }
                                }
                        });

                w.system<const CopyNodeLocalTransformToParent, NodeTransforms, const ModelNodeID, const Transform3D::LocalTranslation, const Transform3D::LocalRotation, const Transform3D::LocalScale>("Copy Node Local Transforms To Parent's NodeTransforms")
                .arg(2).set(flecs::Parent)
                .arg(4).oper(flecs::Optional)
                .arg(5).oper(flecs::Optional)
                .arg(6).oper(flecs::Optional)
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it, const CopyNodeLocalTransformToParent* dummy_for_tag, NodeTransforms* nt, const ModelNodeID* node_id, const Transform3D::LocalTranslation* lt, const Transform3D::LocalRotation* lr, const Transform3D::LocalScale* ls){
                                for (auto i : it)
                                {
                                        glm::mat4 mat = glm::identity<glm::mat4>();
                                        if (lt)
                                                mat *= lt[i].GetMatrix();
                                        if (lr)
                                                mat *= lr[i].GetMatrix();
                                        if (ls)
                                                mat *= ls[i].GetMatrix();
                                        nt[i].NodeTransforms.at(node_id[i].NodeID) = mat;
                                }
                        });

                w.system<const CopyNodeLocalTransformFromParent, const NodeTransforms, const ModelNodeID, Transform3D::LocalTranslation, Transform3D::LocalRotation, Transform3D::LocalScale>("Copy Node Local Transforms From Parent's NodeTransforms")
                .arg(2).set(flecs::Parent)
                .arg(4).oper(flecs::Optional)
                .arg(5).oper(flecs::Optional)
                .arg(6).oper(flecs::Optional)
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it, const CopyNodeLocalTransformFromParent* dummy_for_tag, const NodeTransforms* nt, const ModelNodeID* node_id, Transform3D::LocalTranslation* lt, Transform3D::LocalRotation* lr, Transform3D::LocalScale* ls){
                                glm::vec3 scale;
                                glm::quat rotation;
                                glm::vec3 translation;
                                glm::vec3 skew;
                                glm::vec4 perspective;
                                for (auto i : it)
                                {
                                        glm::decompose(nt[i].NodeTransforms.at(node_id[i].NodeID), scale, rotation, translation, skew, perspective);
                                        if (lt)
                                                lt[i].Translation = translation;
                                        if (lr)
                                                lr[i].Rotation = glm::eulerAngles(rotation);
                                        if (ls)
                                                ls[i].Scale = scale;
                                }
                        });

                w.system<NodeTransforms, const Model>("Convert NodeTransforms to Model Space")
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it, NodeTransforms* nt, const Model* m){
                                for (auto i : it)
                                {
                                        for (unsigned int n = 0; n < m[i].ModelHandle->m_Nodes.size(); n++)
                                        {
                                                if (m[i].ModelHandle->m_Nodes[n].m_ParentNodeID != -1)
                                                        nt[i].NodeTransforms[n] = nt[i].NodeTransforms[m[i].ModelHandle->m_Nodes[n].m_ParentNodeID] * nt[i].NodeTransforms[n];
                                        }
                                }
                        });

                w.system<const CopyNodeLocalTransformToParent, NodeTransforms, const ModelNodeID, const Transform3D::LocalTranslation, const Transform3D::LocalRotation, const Transform3D::LocalScale>("Copy Node Model Space Transforms To Parent's NodeTransforms")
                .arg(2).set(flecs::Parent)
                .arg(4).oper(flecs::Optional)
                .arg(5).oper(flecs::Optional)
                .arg(6).oper(flecs::Optional)
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it, const CopyNodeLocalTransformToParent* dummy_for_tag, NodeTransforms* nt, const ModelNodeID* node_id, const Transform3D::LocalTranslation* lt, const Transform3D::LocalRotation* lr, const Transform3D::LocalScale* ls){
                                for (auto i : it)
                                {
                                        glm::mat4 mat = glm::identity<glm::mat4>();
                                        if (lt)
                                                mat *= lt[i].GetMatrix();
                                        if (lr)
                                                mat *= lr[i].GetMatrix();
                                        if (ls)
                                                mat *= ls[i].GetMatrix();
                                        nt[i].NodeTransforms.at(node_id[i].NodeID) = mat;
                                }
                        });

                w.system<const CopyNodeLocalTransformFromParent, const NodeTransforms, const ModelNodeID, Transform3D::LocalTranslation, Transform3D::LocalRotation, Transform3D::LocalScale>("Copy Node Model Space Transforms From Parent's NodeTransforms")
                .arg(2).set(flecs::Parent)
                .arg(4).oper(flecs::Optional)
                .arg(5).oper(flecs::Optional)
                .arg(6).oper(flecs::Optional)
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it, const CopyNodeLocalTransformFromParent* dummy_for_tag, const NodeTransforms* nt, const ModelNodeID* node_id, Transform3D::LocalTranslation* lt, Transform3D::LocalRotation* lr, Transform3D::LocalScale* ls){
                                glm::vec3 scale;
                                glm::quat rotation;
                                glm::vec3 translation;
                                glm::vec3 skew;
                                glm::vec4 perspective;
                                for (auto i : it)
                                {
                                        glm::decompose(nt[i].NodeTransforms.at(node_id[i].NodeID), scale, rotation, translation, skew, perspective);
                                        if (lt)
                                                lt[i].Translation = translation;
                                        if (lr)
                                                lr[i].Rotation = glm::eulerAngles(rotation);
                                        if (ls)
                                                ls[i].Scale = scale;
                                }
                        });
                w.system<BoneTransforms, const NodeTransforms, const Model, const Transform3D::Translation, const Transform3D::Rotation, const Transform3D::Scale>("Calculate Bone Transform Matricies")
                .arg(4).oper(flecs::Optional)
                .arg(5).oper(flecs::Optional)
                .arg(6).oper(flecs::Optional)
                .kind(w.id<Luddite::PreRender>())
                .iter([](flecs::iter it, BoneTransforms* bt, const NodeTransforms* nt, const Model* m, const Transform3D::Translation* t, const Transform3D::Rotation* r, const Transform3D::Scale* s){
                                for (auto i : it)
                                {
                                        glm::mat4 GlobalInverseTransform = glm::identity<glm::mat4>();
                                        if (t)
                                                GlobalInverseTransform *= t[i].GetMatrix();
                                        if (r)
                                                GlobalInverseTransform *= r[i].GetMatrix();
                                        if (s)
                                                GlobalInverseTransform *= s[i].GetMatrix();
                                        GlobalInverseTransform = glm::inverse(GlobalInverseTransform);
                                        GlobalInverseTransform = m[i].ModelHandle->m_GlobalInverseTransform;
                                        GlobalInverseTransform = glm::inverse(nt[i].NodeTransforms[0]);

                                        for (unsigned int b = 0; b < m[i].ModelHandle->m_Bones.size(); b++)
                                        {
                                                bt[i].BoneTransforms[b] =
                                                        GlobalInverseTransform *
                                                        nt[i].NodeTransforms.at(m[i].ModelHandle->m_Bones[b].m_NodeID) *
                                                        m[i].ModelHandle->m_Bones[b].m_OffsetMatrix;
                                        }
                                }
                        });

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

                w.system<const Transform3D::Translation, const Transform3D::Rotation, const Transform3D::Scale, const Model, const BoneTransforms>("Submit Meshes")
                .arg(5).oper(flecs::Optional)
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it, const Transform3D::Translation* t, const Transform3D::Rotation* r, const Transform3D::Scale* s, const Model* m, const BoneTransforms* bt){
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
