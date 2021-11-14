#include "Luddite/Graphics/VTFSRenderer.hpp"
#include "GraphicsTypes.h"
#include "Luddite/Graphics/Renderer.hpp"
#include "Graphics/GraphicsTools/interface/ShaderMacroHelper.hpp"
#include "Luddite/Core/Profiler.hpp"

using namespace Diligent;

namespace Luddite
{
static const uint32_t gs_NumLevelNodes[] =
{
        1,      // 1st level (32^0)
        32,     // 2nd level (32^1)
        1024,   // 3rd level (32^2)
        32768,  // 4th level (32^3)
        1048576, // 5th level (32^4)
        33554432, // 6th level (32^5)
};
static const uint32_t gs_NumBVHNodes[] =
{
        1,      // 1 level  =32^0
        33,     // 2 levels +32^1
        1057,   // 3 levels +32^2
        33825,  // 4 levels +32^3
        1082401, // 5 levels +32^4
        34636833, // 6 levels +32^5
};

inline uint32_t GetNumLevels(uint32_t numLeaves)
{
        static const float log32f = glm::log(32.0f);
        uint32_t numLevels = 0;
        if (numLeaves > 0)
                numLevels = static_cast<uint32_t>(glm::ceil(glm::log(numLeaves) / log32f));
        return numLevels;
}

inline uint32_t GetNumNodes(uint32_t numLeaves)
{
        uint32_t numLevels = GetNumLevels(numLeaves);
        uint32_t numNodes = 0;
        if (numLevels > 0 && numLevels < _countof(gs_NumBVHNodes))
                numNodes = gs_NumBVHNodes[numLevels - 1];
        return numNodes;
};

void VTFSRenderer::Initialize(Diligent::TEXTURE_FORMAT RTVFormat)
{
        LD_PROFILE_FUNCTION();
        // m_ComputeConstantShaderAttributes.AddUVec3("numThreadGroups");
        // m_ComputeConstantShaderAttributes.AddUInt("padding");
        // m_ComputeConstantShaderAttributes.AddUVec3("numThreads");
        // m_ComputeConstantShaderData = m_ComputeConstantShaderAttributes.CreateData();

        m_CameraCBAttributes.AddMat4("Projection");
        m_CameraCBAttributes.AddMat4("InverseProjection");
        m_CameraCBAttributes.AddMat4("InverseView");
        m_CameraCBAttributes.AddVec2("ViewDimensions");
        m_CameraCBData = m_CameraCBAttributes.CreateData("Camera CB Attributes Data");
        Assets::GetShaderLibrary().static_buffers["_CameraCB"] = m_CameraCB;

        CreateLightBuffers();
        CreateComputePSOs();
        CreateDrawPSOs();
        CreateWholeScreenPSOs();

        Assets::GetShaderLibrary().mutable_buffers.emplace("UniqueClusters");
        Assets::GetShaderLibrary().mutable_buffers.emplace("ClusterFlags");
        Assets::GetShaderLibrary().mutable_buffers.emplace("ClusterAABBs");
        Assets::GetShaderLibrary().mutable_buffers.emplace("PointLightGrid");
        Assets::GetShaderLibrary().mutable_buffers.emplace("SpotLightGrid");
        Assets::GetShaderLibrary().mutable_buffers.emplace("PointLightIndexList");
        Assets::GetShaderLibrary().mutable_buffers.emplace("SpotLightIndexList");

        //Load textures
        Assets::GetShaderLibrary().provided_textures.emplace("g_IrradianceMap");
        Assets::GetShaderLibrary().provided_textures.emplace("g_RadianceMap");
        Assets::GetShaderLibrary().provided_textures.emplace("g_Skybox");
        Handle<Texture> irradiance_map = Assets::GetTextureLibrary().GetAsset(6739077507378108517ULL);
        irradiance_map->TransitionToShaderResource();
        m_TextureMap["g_IrradianceMap"] = irradiance_map;

        Handle<Texture> radiance_map = Assets::GetTextureLibrary().GetAsset(696320760117772384ULL);
        radiance_map->TransitionToShaderResource();
        m_TextureMap["g_RadianceMap"] = radiance_map;

        Handle<Texture> skybox = Assets::GetTextureLibrary().GetAsset(18060254626779487935ULL);
        skybox->TransitionToShaderResource();
        m_TextureMap["g_Skybox"] = skybox;
}

void ClearBufferUInt(IBuffer* buffer)
{
        auto size = buffer->GetDesc().uiSizeInBytes;
        char bytes[size];
        memset(bytes, 0, size);
        Renderer::GetContext()->UpdateBuffer(buffer, 0, size, bytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void ClearBufferUInt(IBuffer* buffer, uint32_t value)
{
        uint32_t size = buffer->GetDesc().uiSizeInBytes;
        uint32_t size_in_uint32s = size / sizeof(uint32_t);
        uint32_t bytes[size_in_uint32s];
        std::fill(bytes, bytes + size_in_uint32s, value);
        Renderer::GetContext()->UpdateBuffer(buffer, 0, size, bytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void ClearBufferFloat(IBuffer* buffer)
{
        uint32_t size = buffer->GetDesc().uiSizeInBytes;
        uint32_t size_in_floats = size / sizeof(float);
        float bytes[size_in_floats];
        std::fill(bytes, bytes + size_in_floats, 0.0f);
        Renderer::GetContext()->UpdateBuffer(buffer, 0, size, bytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void VTFSRenderer::Render(const RenderTarget& render_target, const Camera& camera, const RenderScene& render_scene)
{
        LD_PROFILE_FUNCTION();
        // const auto& RPDesc = m_pRenderPass->GetDesc();
        //Map Camera CB
        glm::mat4 projection = render_target.GetProjectionMatrix(camera);
        glm::mat4 inverse_projection = glm::inverse(projection);
        //Map Camera View Projection Matrix
        glm::mat4 view = render_target.GetViewMatrix(camera);
        glm::mat4 view_projection = projection * view;
        glm::mat4 inverse_view = glm::inverse(view);

        m_CameraCBAttributes.SetMat4(m_CameraCBData, "Projection", projection);
        m_CameraCBAttributes.SetMat4(m_CameraCBData, "InverseProjection", inverse_projection);
        m_CameraCBAttributes.SetMat4(m_CameraCBData, "InverseView", inverse_view);
        m_CameraCBAttributes.SetVec2(m_CameraCBData, "ViewDimensions", glm::vec2(render_target.width, render_target.height));
        m_CameraCBAttributes.MapBuffer(m_CameraCBData, m_CameraCB);
        PerRenderTargetData* data = GetRenderTargetData(render_target, camera, projection);

        auto colorRTV = render_target.RTV;
        auto depthZRTV = data->pDepthZTexture->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
        auto depthDSV = render_target.DSV;
        //Clear Targets

        // Clear Buffers
        ClearBufferUInt(data->ClusterFlagsBuffer);
        ClearBufferUInt(data->UniqueClustersBuffer);
        ClearBufferUInt(data->UniqueClustersCounterBuffer);
        ClearBufferUInt(data->PointLightIndexCounterBuffer);
        ClearBufferUInt(data->SpotLightIndexCounterBuffer);
        ClearBufferUInt(data->PointLightGridBuffer);
        ClearBufferUInt(data->SpotLightGridBuffer);
        ClearBufferUInt(m_pMergePathPartitionsBuffer);
        ClearBufferFloat(m_pPointLightBVHBuffer);
        ClearBufferFloat(m_pSpotLightBVHBuffer);

        // Renderer::GetContext()->buffer


        // Transition Resources
        {
                StateTransitionDesc Barriers[] =
                {
                        {data->ClusterFlagsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {data->UniqueClustersBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {data->UniqueClustersCounterBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {data->PointLightIndexCounterBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {data->SpotLightIndexCounterBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {data->PointLightGridBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {m_pMergePathPartitionsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {m_pPointLightBVHBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true},
                        {m_pSpotLightBVHBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }

        m_ClusterCBAttributes.MapBuffer(m_ClusterCBData, m_ClusterCB);
        m_BasicModelCameraCBAttributes.MapBuffer(m_BasicModelCameraCBData, m_BasicModelCameraCB);

        //Set Render Targets
        {
                ITextureView* Targets[] =
                {
                        depthZRTV
                };
                Renderer::GetContext()->SetRenderTargets(_countof(Targets), Targets, depthDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                Renderer::GetContext()->ClearDepthStencil(depthDSV, CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                float ClearColor[4] = {1.f, 1.f, 1.f, 1.f};
                Renderer::GetContext()->ClearRenderTarget(depthZRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
        //Opaque Geometry Depth Pre-Pass
        {
                Renderer::GetContext()->SetPipelineState(m_pOpaqueDepthPSO);
                Renderer::GetContext()->CommitShaderResources(data->OpaqueDepthSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                for (auto material_pair : render_scene.m_Meshes)
                {
                        for (auto mesh_pair : material_pair.second)
                        {
                                auto& mesh = mesh_pair.first;
                                Uint32 offset = 0;
                                IBuffer* pBuffs[] = {mesh->m_pVertexBuffer};
                                Renderer::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_RESET);
                                Renderer::GetContext()->SetIndexBuffer(mesh->m_pIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
                                auto& transform = mesh_pair.second;

                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "ModelViewProjection", view_projection * transform);
                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "ModelView", view * transform);
                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "Model", transform);
                                m_BasicModelCameraCBAttributes.MapBuffer(m_BasicModelCameraCBData, m_BasicModelCameraCB);

                                DrawIndexedAttribs DrawAttrs;
                                DrawAttrs.NumIndices = mesh->m_nIndicies;
                                DrawAttrs.IndexType = VT_UINT32;
                                DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
                                Renderer::GetContext()->DrawIndexed(DrawAttrs);
                        }
                }
        }

        //Set Render Targets
        Renderer::GetContext()->SetRenderTargets(0, nullptr, depthDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        //Cluster Samples
        {
                Renderer::GetContext()->SetPipelineState(m_pClusterSamplesPSO);
                Renderer::GetContext()->CommitShaderResources(data->ClusterSamplesSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                for (auto material_pair : render_scene.m_Meshes)
                {
                        for (auto mesh_pair : material_pair.second)
                        {
                                auto& mesh = mesh_pair.first;
                                Uint32 offset = 0;
                                IBuffer* pBuffs[] = {mesh->m_pVertexBuffer};
                                Renderer::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_RESET);
                                Renderer::GetContext()->SetIndexBuffer(mesh->m_pIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
                                auto& transform = mesh_pair.second;

                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "ModelViewProjection", view_projection * transform);
                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "ModelView", view * transform);
                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "Model", transform);
                                m_BasicModelCameraCBAttributes.MapBuffer(m_BasicModelCameraCBData, m_BasicModelCameraCB);

                                DrawIndexedAttribs DrawAttrs;
                                DrawAttrs.NumIndices = mesh->m_nIndicies;
                                DrawAttrs.IndexType = VT_UINT32;
                                DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
                                Renderer::GetContext()->DrawIndexed(DrawAttrs);
                        }
                }
        }

        //Resource Transition Barriers
        {
                StateTransitionDesc Barriers[] =
                {
                        {data->ClusterFlagsBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }

        uint32_t max_clusters = data->cluster_dimensions.x * data->cluster_dimensions.y * data->cluster_dimensions.z;
        //Find Unique Samples
        {
                DispatchComputeAttribs DispatchAttribs;
                DispatchAttribs.ThreadGroupCountX = glm::ceil((float)max_clusters / 1024.f);
                Renderer::GetContext()->SetPipelineState(m_pFindUniqueClustersPSO);
                Renderer::GetContext()->CommitShaderResources(data->FindUniqueClustersSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                Renderer::GetContext()->DispatchCompute(DispatchAttribs);
        }
        //Upload Lights
        uint32_t num_point_lights = std::min((uint32_t)render_scene.m_PointLights.size(), MAX_POINT_LIGHTS);
        uint32_t num_spot_lights = std::min((uint32_t)render_scene.m_SpotLights.size(), MAX_SPOT_LIGHTS);
        uint32_t num_directional_lights = std::min((uint32_t)render_scene.m_DirectionalLights.size(), MAX_DIRECTIONAL_LIGHTS);
        {
                //Upload Point Lights
                {
                        PointLightGPU* PointLights = m_pPointLightsBuffer->Allocate(Renderer::GetContext(), num_point_lights);
                        for (int i = 0; i < num_point_lights; i++)
                        {
                                PointLights[i].PositionWS = render_scene.m_PointLights[i].Position;
                                PointLights[i].Color = render_scene.m_PointLights[i].Color;
                                PointLights[i].Range = render_scene.m_PointLights[i].Range;
                                PointLights[i].Intensity = render_scene.m_PointLights[i].Intensity;
                                PointLights[i].Enabled = 1;
                                PointLights[i].Selected = 0;
                        }
                        m_pPointLightsBuffer->Upload();
                }
                //Upload Spot Lights
                {
                        SpotLightGPU* SpotLights = m_pSpotLightsBuffer->Allocate(Renderer::GetContext(), num_spot_lights);
                        for (int i = 0; i < num_spot_lights; i++)
                        {
                                SpotLights[i].PositionWS = render_scene.m_SpotLights[i].Position;
                                SpotLights[i].DirectionWS = render_scene.m_SpotLights[i].Direction;
                                SpotLights[i].Color = render_scene.m_SpotLights[i].Color;
                                SpotLights[i].HalfAngle = render_scene.m_SpotLights[i].HalfAngle;
                                SpotLights[i].Range = render_scene.m_SpotLights[i].Range;
                                SpotLights[i].Intensity = render_scene.m_SpotLights[i].Intensity;
                                SpotLights[i].Enabled = 1;
                                SpotLights[i].Selected = 0;
                        }
                        m_pSpotLightsBuffer->Upload();
                }
                //Upload Directional Lights
                {
                        DirectionalLightGPU* DirectionalLights = m_pDirectionalLightsBuffer->Allocate(Renderer::GetContext(), num_directional_lights);
                        for (int i = 0; i < num_directional_lights; i++)
                        {
                                DirectionalLights[i].DirectionWS = render_scene.m_DirectionalLights[i].Direction;
                                DirectionalLights[i].Color = render_scene.m_DirectionalLights[i].Color;
                                DirectionalLights[i].Intensity = render_scene.m_DirectionalLights[i].Intensity;
                                DirectionalLights[i].Enabled = 1;
                                DirectionalLights[i].Selected = 0;
                        }
                        m_pDirectionalLightsBuffer->Upload();
                }
                m_LightCountsCBAttributes.SetUInt(m_LightCountsCBData, "NumPointLights", num_point_lights);
                m_LightCountsCBAttributes.SetUInt(m_LightCountsCBData, "NumSpotLights", num_spot_lights);
                m_LightCountsCBAttributes.SetUInt(m_LightCountsCBData, "NumDirectionalLights", num_directional_lights);
                m_LightCountsCBAttributes.MapBuffer(m_LightCountsCBData, m_LightCountsCB);
                // m_LightCountsCB
        }
        //Calculate lights' view space properties
        {
                m_UpdateLightsCBAttributes.SetMat4(m_UpdateLightsCBData, "view_matrix", view);
                m_UpdateLightsCBAttributes.MapBuffer(m_UpdateLightsCBData, m_UpdateLightsCB);
                uint32_t num_groups = static_cast<uint32_t>(glm::max(num_point_lights, glm::max(num_spot_lights, num_directional_lights)));
                num_groups = static_cast<uint32_t>(glm::ceil((float)num_groups / 1024.f));
                DispatchComputeAttribs DispatchAttribs;
                Renderer::GetContext()->SetPipelineState(m_pUpdateLightsPSO);
                Renderer::GetContext()->CommitShaderResources(data->UpdateLightsSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                Renderer::GetContext()->DispatchCompute(DispatchAttribs);
        }
        //Reduce Lights AABB
        {
                //  First Pass
                uint32_t numThreadGroups = glm::min<uint32_t>(static_cast<uint32_t>(glm::ceil((float)glm::max(num_point_lights, num_spot_lights) / 512.f)), 512);
                if (numThreadGroups > 0)
                {
                        {
                                m_DispatchParamsCBAttributes.SetUVec3(m_DispatchParamsCBData, "NumThreadGroups", glm::uvec3(numThreadGroups, 1, 1));
                                m_DispatchParamsCBAttributes.SetUVec3(m_DispatchParamsCBData, "NumThreads", glm::uvec3(numThreadGroups * 512, 1, 1));
                                m_DispatchParamsCBAttributes.MapBuffer(m_DispatchParamsCBData, m_DispatchParamsCB);
                                DispatchComputeAttribs DispatchAttribs;
                                DispatchAttribs.ThreadGroupCountX = numThreadGroups;
                                Renderer::GetContext()->SetPipelineState(m_pReduceLightsAABB1PSO);
                                Renderer::GetContext()->CommitShaderResources(data->ReduceLightsAABB1SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                        }
                        //  Second Pass
                        {
                                m_DispatchParamsCBAttributes.SetUVec3(m_DispatchParamsCBData, "NumThreadGroups", glm::uvec3(1, 1, 1));
                                m_DispatchParamsCBAttributes.SetUVec3(m_DispatchParamsCBData, "NumThreads", glm::uvec3(512, 1, 1));
                                m_DispatchParamsCBAttributes.MapBuffer(m_DispatchParamsCBData, m_DispatchParamsCB);
                                m_ReductionParamsCBAttributes.SetUInt(m_ReductionParamsCBData, "NumElements", numThreadGroups);
                                m_ReductionParamsCBAttributes.MapBuffer(m_ReductionParamsCBData, m_ReductionParamsCB);
                                DispatchComputeAttribs DispatchAttribs;
                                Renderer::GetContext()->SetPipelineState(m_pReduceLightsAABB2PSO);
                                Renderer::GetContext()->CommitShaderResources(data->ReduceLightsAABB2SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                        }
                }
        }
        //Compute Light Morton Codes
        {
                uint32_t numThreadGroups = static_cast<uint32_t>(glm::ceil((float)glm::max(num_point_lights, num_spot_lights) / 1024.f));
                if (numThreadGroups > 0)
                {
                        DispatchComputeAttribs DispatchAttribs;
                        DispatchAttribs.ThreadGroupCountX = numThreadGroups;
                        Renderer::GetContext()->SetPipelineState(m_pComputeLightMortonCodesPSO);
                        Renderer::GetContext()->CommitShaderResources(data->ComputeLightMortonCodesSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                        Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                }
        }
        //Sort Morton Codes
        {
                uint32_t chunk_size = SORT_NUM_THREADS_PER_THREAD_GROUP;
                //Radix Sort
                {
                        m_SortParamsCBAttributes.SetUInt(m_SortParamsCBData, "ChunkSize", chunk_size);
                        Renderer::GetContext()->SetPipelineState(m_pRadixSortPSO);
                        //Radix Sort (Point Lights)
                        if (num_point_lights > 0)
                        {
                                m_SortParamsCBAttributes.SetUInt(m_SortParamsCBData, "NumElements", num_point_lights);
                                m_SortParamsCBAttributes.MapBuffer(m_SortParamsCBData, m_SortParamsCB);

                                uint32_t numThreadGroups = static_cast<uint32_t>(glm::ceil((float)num_point_lights / (float)SORT_NUM_THREADS_PER_THREAD_GROUP));

                                DispatchComputeAttribs DispatchAttribs;
                                DispatchAttribs.ThreadGroupCountX = numThreadGroups;
                                Renderer::GetContext()->CommitShaderResources(data->RadixSortPointLightsSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->DispatchCompute(DispatchAttribs);

                                Renderer::GetContext()->CopyBuffer(m_pPointLightMortonCodes_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pPointLightMortonCodesBuffer, 0, m_pPointLightMortonCodesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->CopyBuffer(m_pPointLightIndicies_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pPointLightIndiciesBuffer, 0, m_pPointLightIndiciesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                        }
                        //Radix Sort (Spot Lights)
                        if (num_spot_lights > 0)
                        {
                                m_SortParamsCBAttributes.SetUInt(m_SortParamsCBData, "NumElements", num_spot_lights);
                                m_SortParamsCBAttributes.MapBuffer(m_SortParamsCBData, m_SortParamsCB);

                                uint32_t numThreadGroups = static_cast<uint32_t>(glm::ceil((float)num_spot_lights / (float)SORT_NUM_THREADS_PER_THREAD_GROUP));

                                DispatchComputeAttribs DispatchAttribs;
                                DispatchAttribs.ThreadGroupCountX = numThreadGroups;
                                Renderer::GetContext()->CommitShaderResources(data->RadixSortSpotLightsSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->DispatchCompute(DispatchAttribs);

                                Renderer::GetContext()->CopyBuffer(m_pSpotLightMortonCodes_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pSpotLightMortonCodesBuffer, 0, m_pPointLightMortonCodesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->CopyBuffer(m_pSpotLightIndicies_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pSpotLightIndiciesBuffer, 0, m_pPointLightIndiciesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                        }
                }
                //Merge Sort
                {
                        //Merga Sort (Point Lights)
                        if (num_point_lights > 0)
                        {
                                if (MergeSort(data->MergePathPartitionsPointLightsSRB, data->MergePathPartitionsPointLightsSwappedSRB, data->MergeSortPointLightsSRB,
                                        data->MergeSortPointLightsSwappedSRB, num_point_lights, chunk_size))
                                {
                                        Renderer::GetContext()->CopyBuffer(m_pPointLightMortonCodes_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pPointLightMortonCodesBuffer, 0, m_pPointLightMortonCodesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                        Renderer::GetContext()->CopyBuffer(m_pPointLightIndicies_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pPointLightIndiciesBuffer, 0, m_pPointLightIndiciesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                }
                        }
                        //Merga Sort (Spot Lights)
                        if (num_spot_lights > 0)
                        {
                                if (MergeSort(data->MergePathPartitionsSpotLightsSRB, data->MergePathPartitionsSpotLightsSwappedSRB, data->MergeSortSpotLightsSRB,
                                        data->MergeSortSpotLightsSwappedSRB, num_spot_lights, chunk_size))
                                {
                                        Renderer::GetContext()->CopyBuffer(m_pSpotLightMortonCodes_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pSpotLightMortonCodesBuffer, 0, m_pPointLightMortonCodesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                        Renderer::GetContext()->CopyBuffer(m_pSpotLightIndicies_OUTBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_pSpotLightIndiciesBuffer, 0, m_pPointLightIndiciesBuffer->GetDesc().uiSizeInBytes, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                }
                        }
                }
        }
        //Build Light BVH
        {
                uint32_t num_point_light_levels = GetNumLevels(num_point_lights);
                uint32_t num_spot_light_levels = GetNumLevels(num_spot_lights);
                m_BVHParamsCBAttributes.SetUInt(m_BVHParamsCBData, "PointLightLevels", num_point_light_levels);
                m_BVHParamsCBAttributes.SetUInt(m_BVHParamsCBData, "SpotLightLevels", num_spot_light_levels);
                m_BVHParamsCBAttributes.SetUInt(m_BVHParamsCBData, "ChildLevel", 0);
                m_BVHParamsCBAttributes.MapBuffer(m_BVHParamsCBData, m_BVHParamsCB);

                uint32_t max_leaves = glm::max(num_point_lights, num_spot_lights);
                uint32_t num_thread_groups = static_cast<uint32_t>(glm::ceil((float)max_leaves / (float)BVH_NUM_THREADS));
                if (num_thread_groups)
                {
                        uint32_t max_levels = static_cast<uint32_t>(glm::max(num_point_light_levels, num_spot_light_levels));

                        //Build Bottom BVH
                        {
                                DispatchComputeAttribs DispatchAttribs;
                                DispatchAttribs.ThreadGroupCountX = num_thread_groups;
                                Renderer::GetContext()->SetPipelineState(m_pBuildBVHBottomPSO);
                                Renderer::GetContext()->CommitShaderResources(data->BuildBVHBottomSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                        }
                        //Build N BVH levels
                        if (max_levels > 0)
                        {
                                for (uint32_t level = max_levels - 1; level > 0; --level)
                                {
                                        m_BVHParamsCBAttributes.SetUInt(m_BVHParamsCBData, "ChildLevel", level);
                                        m_BVHParamsCBAttributes.MapBuffer(m_BVHParamsCBData, m_BVHParamsCB);

                                        uint32_t num_child_nodes = gs_NumLevelNodes[level];
                                        num_thread_groups = static_cast<uint32_t>(glm::ceil((float)num_child_nodes / (float)BVH_NUM_THREADS));

                                        DispatchComputeAttribs DispatchAttribs;
                                        DispatchAttribs.ThreadGroupCountX = num_thread_groups;
                                        Renderer::GetContext()->SetPipelineState(m_pBuildBVHTopPSO);
                                        Renderer::GetContext()->CommitShaderResources(data->BuildBVHTopSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                                        Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                                }
                        }
                }
        }
        // Transition Resources
        {
                StateTransitionDesc Barriers[] =
                {
                        // {data->ClusterFlagsBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        {data->UniqueClustersBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        {data->UniqueClustersCounterBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        // {data->PointLightIndexCounterBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        // {data->SpotLightIndexCounterBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        // {data->PointLightGridBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        // {m_pMergePathPartitionsBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        // {m_pPointLightBVHBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true},
                        // {m_pSpotLightBVHBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }
        //Update Indirect Argument Buffers
        {
                DispatchComputeAttribs DispatchAttribs;
                Renderer::GetContext()->SetPipelineState(m_pUpdateIndirectArgumentBuffersPSO);
                Renderer::GetContext()->CommitShaderResources(data->UpdateIndirectArgumentBuffersSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                // Renderer::GetContext()
        }
        // Transition Resources
        {
                StateTransitionDesc Barriers[] =
                {
                        {data->AssignLightsToClustersArgumentBufferBuffer, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_INDIRECT_ARGUMENT, true},
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }
        //Assign Lights To Clusters
        {
                m_LightCountsCBAttributes.MapBuffer(m_LightCountsCBData, m_LightCountsCB);
                DispatchComputeIndirectAttribs DispatchIndirectAttribs;
                //Optimized Method:
                Renderer::GetContext()->SetPipelineState(m_pAssignLightsToClustersPSO);
                Renderer::GetContext()->CommitShaderResources(data->AssignLightsToClustersSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                //Brute Force Method:
                // Renderer::GetContext()->SetPipelineState(m_pAssignLightsToClustersBruteForcePSO);
                // Renderer::GetContext()->CommitShaderResources(data->AssignLightsToClustersBruteForceSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                Renderer::GetContext()->DispatchComputeIndirect(DispatchIndirectAttribs, data->AssignLightsToClustersArgumentBufferBuffer);
        }
        //Set Render Targets
        {
                ITextureView* Targets[] =
                {
                        colorRTV
                };
                Renderer::GetContext()->SetRenderTargets(_countof(Targets), Targets, depthDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                float ClearColor[4] = {0.f, 0.f, 0.f, 1.f};
                Renderer::GetContext()->ClearRenderTarget(colorRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                // Renderer::GetContext()->ClearDepthStencil(depthDSV, CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }

        //Opaque Lighting
        {
                for (auto material_pair : render_scene.m_Meshes)
                {
                        if (!material_pair.first->m_pShader.valid())
                                continue;
                        if (!material_pair.first->m_pShader->m_pPSO)
                                continue;
                        Renderer::GetContext()->SetPipelineState(material_pair.first->m_pShader->m_pPSO);
                        auto it = data->ShaderSRBs.find(material_pair.first->m_pShader);
                        RefCntAutoPtr<IShaderResourceBinding> srb;
                        if (it == data->ShaderSRBs.end())
                        {
                                material_pair.first->m_pShader->m_pPSO->CreateShaderResourceBinding(&srb, true);
                                // if (material_pair.first->m_pShader->vertex_shader_uses_properties)
                                //         srb->GetVariableByName(SHADER_TYPE_VERTEX, "Properties")->Set(material_pair.first->m_pShader->m_PropertiesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
                                // if (material_pair.first->m_pShader->pixel_shader_uses_properties)
                                //         srb->GetVariableByName(SHADER_TYPE_PIXEL, "Properties")->Set(material_pair.first->m_pShader->m_PropertiesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
                                for (const std::string& buffer_name : material_pair.first->m_pShader->m_MutableBuffersVertex)
                                {
                                        LD_LOG_INFO("{}", buffer_name);
                                        auto buff = data->m_BufferMap.find(buffer_name);
                                        if (buff != data->m_BufferMap.end())
                                                srb->GetVariableByName(SHADER_TYPE_VERTEX, buff->first.c_str())->Set(buff->second->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
                                }
                                for (const std::string& buffer_name : material_pair.first->m_pShader->m_MutableBuffersPixel)
                                {
                                        LD_LOG_INFO("{}", buffer_name);
                                        auto buff = data->m_BufferMap.find(buffer_name);
                                        if (buff != data->m_BufferMap.end())
                                                srb->GetVariableByName(SHADER_TYPE_PIXEL, buff->first.c_str())->Set(buff->second->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
                                }
                                //for (const std::string& texture_name : material_pair.first->m_pShader->m_PropertiesBufferDescription.TexturesVertexShader
                                for (const std::string& texture_name : material_pair.first->m_pShader->m_ProvidedTexturesVertex)
                                {
                                        LD_LOG_INFO("{}", texture_name);
                                        auto tex = m_TextureMap.find(texture_name);
                                        if (tex != m_TextureMap.end())
                                                srb->GetVariableByName(SHADER_TYPE_VERTEX, tex->first.c_str())->Set(tex->second->GetTexture()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
                                }
                                for (const std::string& texture_name : material_pair.first->m_pShader->m_ProvidedTexturesPixel)
                                {
                                        LD_LOG_INFO("{}", texture_name);
                                        auto tex = m_TextureMap.find(texture_name);
                                        if (tex != m_TextureMap.end())
                                                srb->GetVariableByName(SHADER_TYPE_PIXEL, tex->first.c_str())->Set(tex->second->GetTexture()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
                                }
                                data->ShaderSRBs.insert({material_pair.first->m_pShader, srb});
                        }
                        else
                                srb = it->second;
                        Renderer::GetContext()->CommitShaderResources(srb, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                        material_pair.first->m_pShader->m_PropertiesBufferDescription.MapBuffer(material_pair.first->m_Properties, material_pair.first->m_pShader->m_PropertiesBuffer);
                        for (auto mesh_pair : material_pair.second)
                        {
                                auto& mesh = mesh_pair.first;
                                Uint32 offset = 0;
                                IBuffer* pBuffs[] = {mesh->m_pVertexBuffer};
                                Renderer::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_RESET);
                                Renderer::GetContext()->SetIndexBuffer(mesh->m_pIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
                                auto& transform = mesh_pair.second;

                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "ModelViewProjection", view_projection * transform);
                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "ModelView", view * transform);
                                m_BasicModelCameraCBAttributes.SetMat4(m_BasicModelCameraCBData, "Model", transform);
                                m_BasicModelCameraCBAttributes.MapBuffer(m_BasicModelCameraCBData, m_BasicModelCameraCB);

                                //For textures
                                // Renderer::GetContext()->CommitShaderResources(m_pCurrentMaterial->m_pMaterialConstantsBuffer->)

                                DrawIndexedAttribs DrawAttrs;
                                DrawAttrs.NumIndices = mesh->m_nIndicies;
                                DrawAttrs.IndexType = VT_UINT32;
                                DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
                                Renderer::GetContext()->DrawIndexed(DrawAttrs);
                        }
                }
        }

        //Depth Debug
        // {
        //         Renderer::GetContext()->SetPipelineState(m_pDebugDepthPSO);
        //         Renderer::GetContext()->CommitShaderResources(data->DebugDepthSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        //         DrawAttribs DrawAttrs;
        //         DrawAttrs.NumVertices = 4;
        //         DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
        //         Renderer::GetContext()->Draw(DrawAttrs);
        // }

        // {
        //         auto pOffscreenRenderTarget = data->pFrameBuffer->GetDesc().ppAttachments[2]->GetTexture();
        //         auto* RTV_texture = render_target.RTV->GetTexture();
        //         CopyTextureAttribs CopyAttribs{pOffscreenRenderTarget, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
        //                                        RTV_texture, RESOURCE_STATE_TRANSITION_MODE_TRANSITION};
        //         Renderer::GetContext()->CopyTexture(CopyAttribs);
        // }
}

VTFSRenderer::PerRenderTargetData VTFSRenderer::CreateRenderTargetData(const RenderTarget& render_target, const Camera& camera)
{
        LD_PROFILE_FUNCTION();
        PerRenderTargetData data;

        // const auto& RPDesc = m_pRenderPass->GetDesc();
        const auto& RTDesc = render_target.RTV->GetDesc();

        TextureDesc TexDesc;
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.Width = render_target.width;
        TexDesc.Height = render_target.height;

        // RefCntAutoPtr<ITexture> pDepthStencilTexture;
        // {
        //         TexDesc.Name = "Depth Stencil Buffer";
        //         TexDesc.Format = RPDesc.pAttachments[0].Format;
        //         TexDesc.BindFlags = BIND_DEPTH_STENCIL;
        //         TexDesc.ClearValue.Format = TexDesc.Format;
        //         TexDesc.ClearValue.DepthStencil.Depth = 1.f;
        //         TexDesc.ClearValue.DepthStencil.Stencil = 0;
        //         Renderer::GetDevice()->CreateTexture(TexDesc, nullptr, &pDepthStencilTexture);
        // }

        // ITextureView* pRTV;
        // if (Renderer::GetDevice()->GetDeviceInfo().IsGLDevice() && render_target.is_swap_chain_buffer)
        // {
        //         TexDesc.Name = "VTFS Render Target";
        //         TexDesc.Format = RPDesc.pAttachments[1].Format;
        //         TexDesc.BindFlags = BIND_RENDER_TARGET;
        //         TexDesc.ClearValue.Format = TexDesc.Format;
        //         TexDesc.ClearValue.Color[0] = 1.f;
        //         TexDesc.ClearValue.Color[1] = 1.f;
        //         TexDesc.ClearValue.Color[2] = 1.f;
        //         TexDesc.ClearValue.Color[3] = 1.f;
        //         Renderer::GetDevice()->CreateTexture(TexDesc, nullptr, &data.pOpenGLRenderTexture);
        //         pRTV = data.pOpenGLRenderTexture->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
        // }
        // else
        // {
        //         pRTV = render_target.RTV;
        // }

        // RefCntAutoPtr<ITexture> pDepthZTexture;
        {
                TexDesc.Name = "Depth Z";
                TexDesc.Format = TEX_FORMAT_R32_FLOAT;
                TexDesc.BindFlags = BIND_RENDER_TARGET | BIND_INPUT_ATTACHMENT;
                TexDesc.ClearValue.Format = TexDesc.Format;
                TexDesc.ClearValue.Color[0] = 1.f;
                TexDesc.ClearValue.Color[1] = 1.f;
                TexDesc.ClearValue.Color[2] = 1.f;
                TexDesc.ClearValue.Color[3] = 1.f;
                Renderer::GetDevice()->CreateTexture(TexDesc, nullptr, &data.pDepthZTexture);
        }

        // TexDesc.BindFlags = BIND_RENDER_TARGET | BIND_INPUT_ATTACHMENT;

        // ITextureView* pAttachments[] =
        // {
        //         pDepthStencilTexture->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL),
        //         pRTV,
        //         pDepthZTexture->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET)
        // };

        // FramebufferDesc FBDesc;
        // FBDesc.Name = "VTFS Frame Buffer";
        // FBDesc.pRenderPass = m_pRenderPass;
        // FBDesc.AttachmentCount = _countof(pAttachments);
        // FBDesc.ppAttachments = pAttachments;
        // Renderer::GetDevice()->CreateFramebuffer(FBDesc, &data.pFrameBuffer);

        //Create SRBs
        m_pOpaqueDepthPSO->CreateShaderResourceBinding(&data.OpaqueDepthSRB, true);

        m_pClusterSamplesPSO->CreateShaderResourceBinding(&data.ClusterSamplesSRB, true);

        m_pFindUniqueClustersPSO->CreateShaderResourceBinding(&data.FindUniqueClustersSRB, true);

        m_pUpdateIndirectArgumentBuffersPSO->CreateShaderResourceBinding(&data.UpdateIndirectArgumentBuffersSRB, true);

        m_pUpdateLightsPSO->CreateShaderResourceBinding(&data.UpdateLightsSRB, true);
        data.UpdateLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.UpdateLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.UpdateLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWDirectionalLights")->Set(m_pDirectionalLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pReduceLightsAABB1PSO->CreateShaderResourceBinding(&data.ReduceLightsAABB1SRB, true);
        data.ReduceLightsAABB1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.ReduceLightsAABB1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.ReduceLightsAABB1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWLightsAABB")->Set(m_pLightsAABBBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pReduceLightsAABB2PSO->CreateShaderResourceBinding(&data.ReduceLightsAABB2SRB, true);
        data.ReduceLightsAABB2SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWLightsAABB")->Set(m_pLightsAABBBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pComputeLightMortonCodesPSO->CreateShaderResourceBinding(&data.ComputeLightMortonCodesSRB, true);
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "LightsAABB")->Set(m_pLightsAABBBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightMortonCodes")->Set(m_pPointLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightMortonCodes")->Set(m_pSpotLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightIndicies")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.ComputeLightMortonCodesSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightIndicies")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pRadixSortPSO->CreateShaderResourceBinding(&data.RadixSortPointLightsSRB, true);
        data.RadixSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pPointLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.RadixSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.RadixSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pPointLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.RadixSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pPointLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));


        m_pRadixSortPSO->CreateShaderResourceBinding(&data.RadixSortSpotLightsSRB, true);
        data.RadixSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pSpotLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.RadixSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.RadixSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pSpotLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.RadixSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pSpotLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pMergePathPartitionsPSO->CreateShaderResourceBinding(&data.MergePathPartitionsPointLightsSRB, true);
        data.MergePathPartitionsPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pPointLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pPointLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        // data.MergePathPartitionsPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pPointLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergePathPartitionsPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWMergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pMergePathPartitionsPSO->CreateShaderResourceBinding(&data.MergePathPartitionsSpotLightsSRB, true);
        data.MergePathPartitionsSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pSpotLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pSpotLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        // data.MergePathPartitionsSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pSpotLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergePathPartitionsSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWMergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pMergeSortPSO->CreateShaderResourceBinding(&data.MergeSortPointLightsSRB, true);
        data.MergeSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pPointLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pPointLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pPointLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortPointLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "MergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        m_pMergeSortPSO->CreateShaderResourceBinding(&data.MergeSortSpotLightsSRB, true);
        data.MergeSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pSpotLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pSpotLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pSpotLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortSpotLightsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "MergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        m_pMergePathPartitionsPSO->CreateShaderResourceBinding(&data.MergePathPartitionsPointLightsSwappedSRB, true);
        data.MergePathPartitionsPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pPointLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pPointLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pPointLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        // data.MergePathPartitionsPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergePathPartitionsPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWMergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pMergePathPartitionsPSO->CreateShaderResourceBinding(&data.MergePathPartitionsSpotLightsSwappedSRB, true);
        data.MergePathPartitionsSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pSpotLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pSpotLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.MergePathPartitionsSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pSpotLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        // data.MergePathPartitionsSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergePathPartitionsSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWMergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pMergeSortPSO->CreateShaderResourceBinding(&data.MergeSortPointLightsSwappedSRB, true);
        data.MergeSortPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pPointLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pPointLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pPointLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortPointLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "MergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        m_pMergeSortPSO->CreateShaderResourceBinding(&data.MergeSortSpotLightsSwappedSRB, true);
        data.MergeSortSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputKeys")->Set(m_pSpotLightMortonCodes_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "InputValues")->Set(m_pSpotLightIndicies_OUTBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.MergeSortSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputKeys")->Set(m_pSpotLightMortonCodesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputValues")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.MergeSortSpotLightsSwappedSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "MergePathPartitions")->Set(m_pMergePathPartitionsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        m_pBuildBVHBottomPSO->CreateShaderResourceBinding(&data.BuildBVHBottomSRB, true);
        data.BuildBVHBottomSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.BuildBVHBottomSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.BuildBVHBottomSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLightIndicies")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.BuildBVHBottomSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLightIndicies")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.BuildBVHBottomSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightBVH")->Set(m_pPointLightBVHBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.BuildBVHBottomSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightBVH")->Set(m_pSpotLightBVHBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pBuildBVHTopPSO->CreateShaderResourceBinding(&data.BuildBVHTopSRB, true);
        data.BuildBVHTopSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightBVH")->Set(m_pPointLightBVHBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.BuildBVHTopSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightBVH")->Set(m_pSpotLightBVHBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        m_pAssignLightsToClustersPSO->CreateShaderResourceBinding(&data.AssignLightsToClustersSRB, true);
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLightBVH")->Set(m_pPointLightBVHBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLightIndicies")->Set(m_pPointLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLightBVH")->Set(m_pSpotLightBVHBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLightIndicies")->Set(m_pSpotLightIndiciesBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        m_pAssignLightsToClustersBruteForcePSO->CreateShaderResourceBinding(&data.AssignLightsToClustersBruteForceSRB, true);
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "PointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "SpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));



        m_pBasicMeshOpaqueLightingPSO->CreateShaderResourceBinding(&data.BasicMeshOpaqueLightingSRB, true);
        data.BasicMeshOpaqueLightingSRB->GetVariableByName(SHADER_TYPE_PIXEL, "PointLights")->Set(m_pPointLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        // data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_PIXEL, "SpotLights")->Set(m_pSpotLightsBuffer->GetBuffer()->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));


        m_pDebugDepthPSO->CreateShaderResourceBinding(&data.DebugDepthSRB, true);
        data.DebugDepthSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_SubpassInputDepthZ")->Set(data.pDepthZTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));

        data.m_BufferMap["PointLights"] = m_pPointLightsBuffer->GetBuffer();
        data.m_BufferMap["SpotLights"] = m_pSpotLightsBuffer->GetBuffer();
        data.m_BufferMap["DirectionalLights"] = m_pDirectionalLightsBuffer->GetBuffer();

        ComputeClusterGrid(data, render_target, camera);
        return data;
}

VTFSRenderer::PerRenderTargetData* VTFSRenderer::GetRenderTargetData(const RenderTarget& render_target, const Camera& camera, const glm::mat4& projection)
{
        LD_PROFILE_FUNCTION();
        auto it = m_PerRenderTargetCache.find(render_target.RTV);
        if (it != m_PerRenderTargetCache.end())
        {
                //Recalculate the cluster grid if needed
                if (projection != it->second.prev_projection_matrix)
                {
                        m_PerRenderTargetCache.erase(it);
                        return GetRenderTargetData(render_target, camera, projection);
                }
                return &it->second;
        }
        auto pair = m_PerRenderTargetCache.emplace(render_target.RTV, CreateRenderTargetData(render_target, camera));
        LD_VERIFY(pair.second, "Failed to Create Render Target Data for VTFS");
        pair.first->second.prev_projection_matrix = projection;
        return &pair.first->second;
}

void VTFSRenderer::ComputeClusterGrid(VTFSRenderer::PerRenderTargetData& data, const RenderTarget& render_target, const Camera& camera)
{
        LD_PROFILE_FUNCTION();
        float fov_y = camera.FOV * 0.5f;
        float z_near = camera.ClipNear;
        float z_far = camera.ClipFar;

        glm::uvec3 cluster_dimensions = glm::ceil(glm::vec3((float)render_target.width / (float)m_ClusterGridBlockSize, (float)render_target.height / (float)m_ClusterGridBlockSize, 1));
        float sD = 2.0f * glm::tan(fov_y) / (float)cluster_dimensions.y;
        float log_dim_y = 1.0f / glm::log(1.0f + sD);
        float log_depth = glm::log(z_far / z_near);
        cluster_dimensions.z = static_cast<uint32_t>(glm::floor(log_depth * log_dim_y));

        data.cluster_dimensions = cluster_dimensions;

        m_ClusterCBAttributes.SetUVec3(m_ClusterCBData, "grid_dim", cluster_dimensions);
        m_ClusterCBAttributes.SetFloat(m_ClusterCBData, "view_near", z_near);
        m_ClusterCBAttributes.SetUVec2(m_ClusterCBData, "size", glm::uvec2(m_ClusterGridBlockSize, m_ClusterGridBlockSize));
        m_ClusterCBAttributes.SetFloat(m_ClusterCBData, "near_k", 1.0f + sD);
        m_ClusterCBAttributes.SetFloat(m_ClusterCBData, "log_grid_dim_y", log_dim_y);
        m_ClusterCBAttributes.MapBuffer(m_ClusterCBData, m_ClusterCB);
        //.Assets::GetShaderLibrary().static_buffers["

        BufferDesc BuffDesc;
        BuffDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
        BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
        BuffDesc.Usage = USAGE_DEFAULT;
        // BuffDesc.CPUAccessFlags

        data.UniqueClustersBuffer.Release();
        data.ClusterFlagsBuffer.Release();
        data.ClusterAABBsBuffer.Release();
        data.PointLightGridBuffer.Release();
        data.SpotLightGridBuffer.Release();
        data.SpotLightIndexListBuffer.Release();
        data.PointLightIndexListBuffer.Release();
        data.ComputeClusterGridSRB.Release();

        BuffDesc.ElementByteStride = sizeof(uint32_t);
        BuffDesc.uiSizeInBytes = sizeof(uint32_t) * cluster_dimensions.x * cluster_dimensions.y * cluster_dimensions.z;
        BuffDesc.Name = "Unique Clusters";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.UniqueClustersBuffer);
        data.m_BufferMap["UniqueClusters"] = data.UniqueClustersBuffer;
        data.FindUniqueClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWUniqueClusters")->Set(data.UniqueClustersBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "UniqueClusters")->Set(data.UniqueClustersBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "UniqueClusters")->Set(data.UniqueClustersBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        BuffDesc.Name = "Clusters Flags";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.ClusterFlagsBuffer);
        data.m_BufferMap["ClusterFlags"] = data.ClusterFlagsBuffer;
        data.ClusterSamplesSRB->GetVariableByName(SHADER_TYPE_PIXEL, "RWClusterFlags")->Set(data.ClusterFlagsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.FindUniqueClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "ClusterFlags")->Set(data.ClusterFlagsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        BuffDesc.ElementByteStride = sizeof(AABB);
        BuffDesc.uiSizeInBytes = sizeof(AABB) * cluster_dimensions.x * cluster_dimensions.y * cluster_dimensions.z;
        BuffDesc.Name = "Cluster AABBs";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.ClusterAABBsBuffer);
        data.m_BufferMap["ClusterAABBs"] = data.ClusterAABBsBuffer;
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "ClusterAABBs")->Set(data.ClusterAABBsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "ClusterAABBs")->Set(data.ClusterAABBsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        //cluster colors buffer (for debug)?

        BuffDesc.ElementByteStride = sizeof(glm::uvec2);
        BuffDesc.uiSizeInBytes = sizeof(glm::uvec2) * cluster_dimensions.x * cluster_dimensions.y * cluster_dimensions.z;
        BuffDesc.Name = "Point Light Grid";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.PointLightGridBuffer);
        data.m_BufferMap["PointLightGrid"] = data.PointLightGridBuffer;
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightGrid")->Set(data.PointLightGridBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightGrid")->Set(data.PointLightGridBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.BasicMeshOpaqueLightingSRB->GetVariableByName(SHADER_TYPE_PIXEL, "PointLightGrid")->Set(data.PointLightGridBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        BuffDesc.Name = "Spot Light Grid";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.SpotLightGridBuffer);
        data.m_BufferMap["SpotLightGrid"] = data.SpotLightGridBuffer;
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightGrid")->Set(data.SpotLightGridBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightGrid")->Set(data.SpotLightGridBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        BuffDesc.ElementByteStride = sizeof(uint32_t);
        BuffDesc.uiSizeInBytes = sizeof(uint32_t) * AVERAGE_OVERLAPPING_LIGHTS_PER_CLUSTER * cluster_dimensions.x * cluster_dimensions.y * cluster_dimensions.z;
        BuffDesc.Name = "Point Light Index List";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.PointLightIndexListBuffer);
        data.m_BufferMap["PointLightIndexList"] = data.PointLightIndexListBuffer;
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightIndexList")->Set(data.PointLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightIndexList")->Set(data.PointLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightIndexList")->Set(data.PointLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.BasicMeshOpaqueLightingSRB->GetVariableByName(SHADER_TYPE_PIXEL, "PointLightIndexList")->Set(data.PointLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        BuffDesc.Name = "Spot Light Index List";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.SpotLightIndexListBuffer);
        data.m_BufferMap["SpotLightIndexList"] = data.SpotLightIndexListBuffer;
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightIndexList")->Set(data.SpotLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightIndexList")->Set(data.SpotLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        // data.BasicMeshOpaqueLightingSRB->GetVariableByName(SHADER_TYPE_PIXEL, "SpotLightIndexList")->Set(data.SpotLightIndexListBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        m_pComputeClusterGridPSO->CreateShaderResourceBinding(&data.ComputeClusterGridSRB, true);
        data.ComputeClusterGridSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWClusterAABBs")->Set(data.ClusterAABBsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        BuffDesc.ElementByteStride = sizeof(uint32_t);
        BuffDesc.uiSizeInBytes = sizeof(uint32_t);

        BuffDesc.Name = "Point Light Index Counter Buffer";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.PointLightIndexCounterBuffer);
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightIndexCounter")->Set(data.PointLightIndexCounterBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWPointLightIndexCounter")->Set(data.PointLightIndexCounterBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        BuffDesc.Name = "Spot Light Index Counter Buffer";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.SpotLightIndexCounterBuffer);
        data.AssignLightsToClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightIndexCounter")->Set(data.SpotLightIndexCounterBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.AssignLightsToClustersBruteForceSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSpotLightIndexCounter")->Set(data.SpotLightIndexCounterBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        BuffDesc.Mode = BUFFER_MODE_RAW;
        BuffDesc.Name = "Unique Clusters Counter";
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.UniqueClustersCounterBuffer);
        data.FindUniqueClustersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWUniqueClusters@count")->Set(data.UniqueClustersCounterBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
        data.UpdateIndirectArgumentBuffersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "ClusterCounter")->Set(data.UniqueClustersCounterBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

        // BuffDesc.CPUAccessFlags = CPU_ACCESS_READ;
        BuffDesc.ElementByteStride = sizeof(uint32_t);
        BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_INDIRECT_DRAW_ARGS;
        BuffDesc.Mode = BUFFER_MODE_RAW;
        BuffDesc.Usage = USAGE_DEFAULT;
        BuffDesc.uiSizeInBytes = sizeof(uint32_t) * 3;
        BuffDesc.Name = "Assign Lights to Clusters Argument Buffer";

        BufferData BuffData;
        glm::uvec3 buffer_data = glm::uvec3(1);
        BuffData.DataSize = sizeof(glm::uvec3);
        BuffData.pData = glm::value_ptr(buffer_data);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, &BuffData, &data.AssignLightsToClustersArgumentBufferBuffer);
        // BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
        data.UpdateIndirectArgumentBuffersSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "AssignLightsToClustersIndirectArgumentBuffer")->Set(data.AssignLightsToClustersArgumentBufferBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        DispatchComputeAttribs DispatchAttribs;
        DispatchAttribs.ThreadGroupCountX = static_cast<uint32_t>(glm::ceil((float)(cluster_dimensions.x * cluster_dimensions.y * cluster_dimensions.z) / 1024.f));
        Renderer::GetContext()->SetPipelineState(m_pComputeClusterGridPSO);
        Renderer::GetContext()->CommitShaderResources(data.ComputeClusterGridSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        Renderer::GetContext()->DispatchCompute(DispatchAttribs);

        // glm::uvec3 threads = glm::ceil(glm::vec3((float)render_target.width / (float)m_BlockSize, (float)render_target.height / (float)m_BlockSize, 1));
        // glm::uvec3 thread_groups = glm::ceil(glm::vec3((float)threads.x / (float)m_BlockSize, (float)threads.y / (float)m_BlockSize, 1));



        // data.GridFrustumsBuffer.Release();
        // Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &data.GridFrustumsBuffer);

        // data.ComputeGridFrustumsSRB.Release();
        // m_pComputeGridFrustumsPSO->CreateShaderResourceBinding(&data.ComputeGridFrustumsSRB, true);
        // data.ComputeGridFrustumsSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "g_Frustums")->Set(data.GridFrustumsBuffer->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));

        // m_ComputeConstantShaderAttributes.SetUVec3(m_ComputeConstantShaderData, "numThreads", threads);
        // m_ComputeConstantShaderAttributes.SetUVec3(m_ComputeConstantShaderData, "numThreadGroups", thread_groups);
        // m_ComputeConstantShaderAttributes.MapBuffer(m_ComputeConstantShaderData, m_ComputeConstants);
}

bool VTFSRenderer::MergeSort(Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergePathPartitionsSRB,
                             Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergePathPartitionsSwappedSRB,
                             Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergeSortSRB,
                             Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergeSortSwappedSRB,
                             uint32_t total_values, uint32_t chunk_size)
{
        LD_PROFILE_FUNCTION();
        m_SortParamsCBAttributes.SetUInt(m_SortParamsCBData, "NumElements", total_values);
        m_SortParamsCBAttributes.SetUInt(m_SortParamsCBData, "ChunkSize", chunk_size);
        m_SortParamsCBAttributes.MapBuffer(m_SortParamsCBData, m_SortParamsCB);

        uint32_t num_thread_groups;

        const uint32_t num_threads_per_thread_group = SORT_NUM_THREADS_PER_THREAD_GROUP;
        const uint32_t num_values_per_thread = SORT_ELEMENTS_PER_THREAD;
        const uint32_t num_values_per_thread_group = num_threads_per_thread_group * num_values_per_thread;
        uint32_t num_chunks = static_cast<uint32_t>(glm::ceil((float)total_values / (float)chunk_size));

        uint32_t pass = 0;
        while (num_chunks > 1)
        {
                pass++;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pass_pMergePathPartitionsSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pass_pMergeSortSRB;
                if (pass % 2 == 1)
                {
                        pass_pMergePathPartitionsSRB = pMergePathPartitionsSRB;
                        pass_pMergeSortSRB = pMergeSortSRB;
                }
                else
                {
                        pass_pMergePathPartitionsSRB = pMergePathPartitionsSwappedSRB;
                        pass_pMergeSortSRB = pMergeSortSwappedSRB;
                }

                uint32_t num_sort_groups = num_chunks / 2u;
                uint32_t num_thread_groups_per_sort_group = static_cast<uint32_t>(glm::ceil((float)(chunk_size * 2) / (float)num_values_per_thread_group));

                //Compute merge path partitions
                {
                        uint32_t num_merge_path_partitions_per_sort_group = num_threads_per_thread_group + 1;
                        uint32_t total_merge_path_partitions = num_merge_path_partitions_per_sort_group * num_sort_groups;

                        Renderer::GetContext()->SetPipelineState(m_pMergePathPartitionsPSO);
                        Renderer::GetContext()->CommitShaderResources(pass_pMergePathPartitionsSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                        num_thread_groups = static_cast<uint32_t>(glm::ceil((float)total_merge_path_partitions / (float)num_threads_per_thread_group));
                        DispatchComputeAttribs DispatchAttribs;
                        DispatchAttribs.ThreadGroupCountX = num_thread_groups;
                        Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                }

                //Perform Merge Sort
                {
                        uint32_t num_values_per_sort_group = glm::min(chunk_size * 2, total_values);
                        uint32_t num_thread_groups_per_sort_group = static_cast<uint32_t>(glm::ceil((float)num_values_per_sort_group / (float)num_values_per_thread_group));

                        Renderer::GetContext()->SetPipelineState(m_pMergeSortPSO);
                        Renderer::GetContext()->CommitShaderResources(pass_pMergeSortSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                        DispatchComputeAttribs DispatchAttribs;
                        DispatchAttribs.ThreadGroupCountX = num_thread_groups_per_sort_group * num_sort_groups;
                        Renderer::GetContext()->DispatchCompute(DispatchAttribs);
                }

                chunk_size *= 2;
                uint32_t num_chunks = static_cast<uint32_t>(glm::ceil((float)total_values / (float)chunk_size));
        }

        return pass % 2 == 1;
}
void VTFSRenderer::CreateLightBuffers()
{
        LD_PROFILE_FUNCTION();
        m_pPointLightsBuffer = std::make_unique<StreamingBuffer<PointLightGPU> >(Renderer::GetDevice(), BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, BUFFER_MODE_STRUCTURED, MAX_POINT_LIGHTS, "Point Lights Buffer");
        Assets::GetShaderLibrary().mutable_buffers.emplace("PointLights");
        m_pSpotLightsBuffer = std::make_unique<StreamingBuffer<SpotLightGPU> >(Renderer::GetDevice(), BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, BUFFER_MODE_STRUCTURED, MAX_SPOT_LIGHTS, "Spot Lights Buffer");
        Assets::GetShaderLibrary().mutable_buffers.emplace("SpotLights");
        m_pDirectionalLightsBuffer = std::make_unique<StreamingBuffer<DirectionalLightGPU> >(Renderer::GetDevice(), BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, BUFFER_MODE_STRUCTURED, MAX_DIRECTIONAL_LIGHTS, "Directional Lights Buffer");
        Assets::GetShaderLibrary().mutable_buffers.emplace("DirectionalLights");

        BufferDesc BuffDesc;
        BuffDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
        BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
        BuffDesc.Usage = USAGE_DEFAULT;

        BuffDesc.ElementByteStride = sizeof(AABB);
        BuffDesc.uiSizeInBytes = sizeof(AABB) * 512;
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pLightsAABBBuffer);

        BuffDesc.uiSizeInBytes = sizeof(AABB) * GetNumNodes(MAX_POINT_LIGHTS);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pPointLightBVHBuffer);

        BuffDesc.uiSizeInBytes = sizeof(AABB) * GetNumNodes(MAX_SPOT_LIGHTS);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pSpotLightBVHBuffer);

        BuffDesc.ElementByteStride = sizeof(uint32_t);
        BuffDesc.uiSizeInBytes = sizeof(uint32_t) * MAX_POINT_LIGHTS;
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pPointLightIndiciesBuffer);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pPointLightIndicies_OUTBuffer);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pPointLightMortonCodesBuffer);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pPointLightMortonCodes_OUTBuffer);

        BuffDesc.uiSizeInBytes = sizeof(uint32_t) * MAX_SPOT_LIGHTS;
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pSpotLightIndiciesBuffer);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pSpotLightIndicies_OUTBuffer);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pSpotLightMortonCodesBuffer);
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pSpotLightMortonCodes_OUTBuffer);

        constexpr static uint32_t maxElements = std::max(MAX_POINT_LIGHTS, MAX_SPOT_LIGHTS);
        uint32_t chunkSize = SORT_NUM_THREADS_PER_THREAD_GROUP;
        uint32_t numChunks = static_cast<uint32_t>(glm::ceil((float)maxElements / (float)chunkSize));
        uint32_t maxSortGroups = numChunks / 2;
        uint32_t numMergePathPartitionsPerSortGroup = static_cast<uint32_t>(glm::ceil((float)(chunkSize * 2) / (float)(SORT_ELEMENTS_PER_THREAD * SORT_NUM_THREADS_PER_THREAD_GROUP))) + 1u;
        uint32_t maxMergePathPartitions = numMergePathPartitionsPerSortGroup * maxSortGroups;

        BuffDesc.uiSizeInBytes = sizeof(uint32_t) * maxMergePathPartitions;
        Renderer::GetDevice()->CreateBuffer(BuffDesc, nullptr, &m_pMergePathPartitionsBuffer);
}

void VTFSRenderer::CreateComputePSOs()
{
        LD_PROFILE_FUNCTION();
        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();

        // ShaderMacroHelper macros;
        // macros.AddShaderMacro("BLOCK_SIZE", m_BlockSize);
        // macros.Finalize();

        RefCntAutoPtr<IShader> pComputeClusterGridCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Compute Cluster Grid AABBs CS";
                ShaderCI.FilePath = "VTFS/ComputeClusterAABBs.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pComputeClusterGridCS);
        }
        RefCntAutoPtr<IShader> pFindUniqueClustersCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Find Unique Clusters CS";
                ShaderCI.FilePath = "VTFS/FindUniqueClusters.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pFindUniqueClustersCS);
        }
        RefCntAutoPtr<IShader> pUpdateIndirectArgumentBuffersCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Update Indirect Arguemnt Buffers CS";
                ShaderCI.FilePath = "VTFS/UpdateIndirectArgumentBuffers.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pUpdateIndirectArgumentBuffersCS);
        }
        RefCntAutoPtr<IShader> pUpdateLightsCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Update Lights CS";
                ShaderCI.FilePath = "VTFS/UpdateLights.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pUpdateLightsCS);
        }
        RefCntAutoPtr<IShader> pReduceLightsAABB1CS;
        RefCntAutoPtr<IShader> pReduceLightsAABB2CS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.FilePath = "VTFS/ReduceLightsAABB.hlsl";
                ShaderCI.Desc.Name = "Reduce Light AABB 1 CS";
                ShaderCI.EntryPoint = "reduce1";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pReduceLightsAABB1CS);
                ShaderCI.Desc.Name = "Reduce Light AABB 2 CS";
                ShaderCI.EntryPoint = "reduce2";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pReduceLightsAABB2CS);
        }
        RefCntAutoPtr<IShader> pComputeLightMortonCodesCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Compute Light Morton Codes CS";
                ShaderCI.FilePath = "VTFS/ComputeLightMortonCodes.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pComputeLightMortonCodesCS);
        }
        RefCntAutoPtr<IShader> pRadixSortCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Radix Sort CS";
                ShaderCI.FilePath = "VTFS/RadixSort.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pRadixSortCS);
        }
        RefCntAutoPtr<IShader> pMergePathPartitionsCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "merge_path_partitions";
                ShaderCI.Desc.Name = "Merge Sort CS";
                ShaderCI.FilePath = "VTFS/MergeSort.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pMergePathPartitionsCS);
        }
        RefCntAutoPtr<IShader> pMergeSortCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "merge_sort";
                ShaderCI.Desc.Name = "Merge Sort CS";
                ShaderCI.FilePath = "VTFS/MergeSort.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pMergeSortCS);
        }
        RefCntAutoPtr<IShader> pBuildBVHBottomCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "BuildBottom";
                ShaderCI.Desc.Name = "Merge Sort CS";
                ShaderCI.FilePath = "VTFS/BuildLightBVH.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBuildBVHBottomCS);
        }
        RefCntAutoPtr<IShader> pBuildBVHTopCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "BuildTop";
                ShaderCI.Desc.Name = "Merge Sort CS";
                ShaderCI.FilePath = "VTFS/BuildLightBVH.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBuildBVHTopCS);
        }
        RefCntAutoPtr<IShader> pAssignLightsToClustersCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Assign Lights to Clusters CS";
                ShaderCI.FilePath = "VTFS/AssignLightsToClusters.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pAssignLightsToClustersCS);
        }
        //for debugging
        RefCntAutoPtr<IShader> pAssignLightsToClustersBruteForceCS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Assign Lights to Clusters Brute Force CS";
                ShaderCI.FilePath = "VTFS/AssignLightsToClustersBruteForce.hlsl";
                // ShaderCI.Macros = macros;
                Renderer::GetDevice()->CreateShader(ShaderCI, &pAssignLightsToClustersBruteForceCS);
        }

        ComputePipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;

        PSODesc.PipelineType = PIPELINE_TYPE_COMPUTE;
        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;
        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_ClusterCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "_CameraCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "RWClusterAABBs", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Compute Grid Frustums PSO";
                PSOCreateInfo.pCS = pComputeClusterGridCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pComputeClusterGridPSO);

                m_ClusterCBAttributes.AddUVec3("grid_dim");
                m_ClusterCBAttributes.AddFloat("view_near");
                m_ClusterCBAttributes.AddUVec2("size");
                m_ClusterCBAttributes.AddFloat("near_k");
                m_ClusterCBAttributes.AddFloat("log_grid_dim_y");
                m_ClusterCBData = m_ClusterCBAttributes.CreateData("Cluster CB Data");

                // m_ClusterCB.Release();
                CreateUniformBuffer(Renderer::GetDevice(), m_ClusterCBAttributes.GetSize(), "_ClusterCB", &m_ClusterCB);
                Assets::GetShaderLibrary().static_buffers["_ClusterCB"] = m_ClusterCB;
                // m_CameraCB.Release();
                CreateUniformBuffer(Renderer::GetDevice(), m_CameraCBAttributes.GetSize(), "_CameraCB", &m_CameraCB);
                Assets::GetShaderLibrary().static_buffers["_CameraCB"] = m_CameraCB;

                StateTransitionDesc Barriers[] =
                {
                        {m_ClusterCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true},
                        {m_CameraCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);

                m_pComputeClusterGridPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_ClusterCB")->Set(m_ClusterCB);
                m_pComputeClusterGridPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_CameraCB")->Set(m_CameraCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "ClusterFlags", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWUniqueClusters", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Find Unique Clusters PSO";
                PSOCreateInfo.pCS = pFindUniqueClustersCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pFindUniqueClustersPSO);
                LD_VERIFY(m_pFindUniqueClustersPSO, "Failed to create {}", PSODesc.Name);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "ClusterCounter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "AssignLightsToClustersIndirectArgumentBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Update Indirect Argument Buffers PSO";
                PSOCreateInfo.pCS = pUpdateIndirectArgumentBuffersCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pUpdateIndirectArgumentBuffersPSO);
                LD_VERIFY(m_pUpdateIndirectArgumentBuffersPSO, "Failed to create {}", PSODesc.Name);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_LightCountsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "UpdateLightsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "RWDirectionalLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Update Lights PSO";
                PSOCreateInfo.pCS = pUpdateLightsCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pUpdateLightsPSO);
                LD_VERIFY(m_pUpdateLightsPSO, "Failed to create {}", PSODesc.Name);

                m_LightCountsCBAttributes.AddUInt("NumPointLights");
                m_LightCountsCBAttributes.AddUInt("NumSpotLights");
                m_LightCountsCBAttributes.AddUInt("NumDirectionalLights");
                m_LightCountsCBData = m_LightCountsCBAttributes.CreateData("Light Counts CB Data");

                m_UpdateLightsCBAttributes.AddMat4("view_matrix");
                m_UpdateLightsCBData = m_UpdateLightsCBAttributes.CreateData("Update Lights CB Data");

                CreateUniformBuffer(Renderer::GetDevice(), m_LightCountsCBAttributes.GetSize(), "_LightCountsCB", &m_LightCountsCB);
                Assets::GetShaderLibrary().static_buffers["_LightCountsCB"] = m_LightCountsCB;
                CreateUniformBuffer(Renderer::GetDevice(), m_UpdateLightsCBAttributes.GetSize(), "UpdateLightsCB", &m_UpdateLightsCB);
                StateTransitionDesc Barriers[] =
                {
                        {m_UpdateLightsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true},
                        {m_LightCountsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
                m_pUpdateLightsPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_LightCountsCB")->Set(m_LightCountsCB);
                m_pUpdateLightsPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "UpdateLightsCB")->Set(m_UpdateLightsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_DispatchParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "_LightCountsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "RWLightsAABB", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Reduce Light AABB 1 PSO";
                PSOCreateInfo.pCS = pReduceLightsAABB1CS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pReduceLightsAABB1PSO);
                LD_VERIFY(m_pReduceLightsAABB1PSO, "Failed to create {}", PSODesc.Name);

                m_DispatchParamsCBAttributes.AddUVec3("NumThreadGroups");
                m_DispatchParamsCBAttributes.AddUVec3("NumThreads");
                m_DispatchParamsCBData = m_DispatchParamsCBAttributes.CreateData("Dispatch Params CB Data");

                CreateUniformBuffer(Renderer::GetDevice(), m_DispatchParamsCBAttributes.GetSize(), "_DispatchParamsCB", &m_DispatchParamsCB);
                StateTransitionDesc Barriers[] =
                {
                        {m_DispatchParamsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
                m_pReduceLightsAABB1PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_DispatchParamsCB")->Set(m_DispatchParamsCB);
                m_pReduceLightsAABB1PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_LightCountsCB")->Set(m_LightCountsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_DispatchParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "_ReductionParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "RWLightsAABB", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Reduce Light AABB 2 PSO";
                PSOCreateInfo.pCS = pReduceLightsAABB2CS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pReduceLightsAABB2PSO);
                LD_VERIFY(m_pReduceLightsAABB2PSO, "Failed to create {}", PSODesc.Name);

                m_ReductionParamsCBAttributes.AddUInt("NumElements");
                m_ReductionParamsCBData = m_ReductionParamsCBAttributes.CreateData("Reduction Params CB Data");

                CreateUniformBuffer(Renderer::GetDevice(), m_DispatchParamsCBAttributes.GetSize(), "_ReductionParamsCB", &m_ReductionParamsCB);
                StateTransitionDesc Barriers[] =
                {
                        {m_ReductionParamsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
                m_pReduceLightsAABB2PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_DispatchParamsCB")->Set(m_DispatchParamsCB);
                m_pReduceLightsAABB2PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_ReductionParamsCB")->Set(m_ReductionParamsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_LightCountsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "LightsAABB", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightMortonCodes", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightMortonCodes", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightIndicies", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightIndicies", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Compute Light Morton Codes PSO";
                PSOCreateInfo.pCS = pComputeLightMortonCodesCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pComputeLightMortonCodesPSO);
                LD_VERIFY(m_pComputeLightMortonCodesPSO, "Failed to create {}", PSODesc.Name);

                m_pComputeLightMortonCodesPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_LightCountsCB")->Set(m_LightCountsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_SortParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "InputKeys", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "InputValues", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "OutputKeys", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "OutputValues", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Radix Sort PSO";
                PSOCreateInfo.pCS = pRadixSortCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pRadixSortPSO);
                LD_VERIFY(m_pRadixSortPSO, "Failed to create {}", PSODesc.Name);

                m_SortParamsCBAttributes.AddUInt("NumElements");
                m_SortParamsCBAttributes.AddUInt("ChunkSize");
                m_SortParamsCBData = m_SortParamsCBAttributes.CreateData("Sort Params CB Data");

                CreateUniformBuffer(Renderer::GetDevice(), m_SortParamsCBAttributes.GetSize(), "_SortParamsCB", &m_SortParamsCB);
                StateTransitionDesc Barriers[] =
                {
                        {m_SortParamsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
                m_pRadixSortPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_SortParamsCB")->Set(m_SortParamsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_SortParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "InputKeys", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        // {SHADER_TYPE_COMPUTE, "InputValues", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        // {SHADER_TYPE_COMPUTE, "OutputKeys", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        // {SHADER_TYPE_COMPUTE, "OutputValues", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWMergePathPartitions", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Merge Path Partitions PSO";
                PSOCreateInfo.pCS = pMergePathPartitionsCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pMergePathPartitionsPSO);
                LD_VERIFY(m_pMergePathPartitionsPSO, "Failed to create {}", PSODesc.Name);

                m_pMergePathPartitionsPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_SortParamsCB")->Set(m_SortParamsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_SortParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "InputKeys", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "InputValues", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "OutputKeys", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "OutputValues", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "MergePathPartitions", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Merge Sort PSO";
                PSOCreateInfo.pCS = pMergeSortCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pMergeSortPSO);
                LD_VERIFY(m_pMergeSortPSO, "Failed to create {}", PSODesc.Name);

                m_pMergeSortPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_SortParamsCB")->Set(m_SortParamsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_BVHParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "_LightCountsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "PointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLightIndicies", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLightIndicies", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightBVH", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightBVH", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                m_BVHParamsCBAttributes.AddUInt("PointLightLevels");
                m_BVHParamsCBAttributes.AddUInt("SpotLightLevels");
                m_BVHParamsCBAttributes.AddUInt("ChildLevel");
                m_BVHParamsCBData = m_BVHParamsCBAttributes.CreateData("BVH Params CB Data");

                PSODesc.Name = "Build BVH Bottom PSO";
                PSOCreateInfo.pCS = pBuildBVHBottomCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pBuildBVHBottomPSO);
                LD_VERIFY(m_pBuildBVHBottomPSO, "Failed to create {}", PSODesc.Name);

                CreateUniformBuffer(Renderer::GetDevice(), m_BVHParamsCBAttributes.GetSize(), "_BVHParamsCB", &m_BVHParamsCB);
                StateTransitionDesc Barriers[] =
                {
                        {m_BVHParamsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);

                uint32_t test = m_pBuildBVHBottomPSO->GetStaticVariableCount(SHADER_TYPE_COMPUTE);

                m_pBuildBVHBottomPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_BVHParamsCB")->Set(m_BVHParamsCB);
                m_pBuildBVHBottomPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_LightCountsCB")->Set(m_LightCountsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_BVHParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "PointLightBVH", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLightBVH", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Build BVH Top PSO";
                PSOCreateInfo.pCS = pBuildBVHTopCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pBuildBVHTopPSO);
                LD_VERIFY(m_pBuildBVHTopPSO, "Failed to create {}", PSODesc.Name);

                m_pBuildBVHTopPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_BVHParamsCB")->Set(m_BVHParamsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_BVHParamsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "_LightCountsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "RWPointLightIndexCounter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightIndexList", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightGrid", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightIndexCounter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightIndexList", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightGrid", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLightBVH", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLightIndicies", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLightBVH", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLightIndicies", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "UniqueClusters", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "ClusterAABBs", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Assign Lights to Clusters PSO";
                PSOCreateInfo.pCS = pAssignLightsToClustersCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pAssignLightsToClustersPSO);
                LD_VERIFY(m_pAssignLightsToClustersPSO, "Failed to create {}", PSODesc.Name);

                m_pAssignLightsToClustersPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_BVHParamsCB")->Set(m_BVHParamsCB);
                m_pAssignLightsToClustersPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_LightCountsCB")->Set(m_LightCountsCB);
        }

        {
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_COMPUTE, "_LightCountsCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_COMPUTE, "RWPointLightIndexCounter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightIndexList", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWPointLightGrid", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightIndexCounter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightIndexList", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "RWSpotLightGrid", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "UniqueClusters", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "ClusterAABBs", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "PointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_COMPUTE, "SpotLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                PSODesc.Name = "Assign Lights to Clusters Brute Force PSO";
                PSOCreateInfo.pCS = pAssignLightsToClustersBruteForceCS;
                Renderer::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_pAssignLightsToClustersBruteForcePSO);
                LD_VERIFY(m_pAssignLightsToClustersBruteForcePSO, "Failed to create {}", PSODesc.Name);

                m_pAssignLightsToClustersBruteForcePSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_LightCountsCB")->Set(m_LightCountsCB);
        }
}

void VTFSRenderer::CreateDrawPSOs()
{
        LD_PROFILE_FUNCTION();
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
        // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_GREATER_EQUAL;
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = Renderer::GetDefaultDSVFormat();
        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = VertexLayoutElements;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(VertexLayoutElements);

        SamplerDesc SamLinearClampDesc
        {
                FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
                TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };

        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();

        RefCntAutoPtr<IShader> pBasicMeshPosOnlyVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Basic Mesh Pos Only VS";
                ShaderCI.FilePath = "VTFS/BasicMeshPosOnly.vsh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBasicMeshPosOnlyVS);
        }
        RefCntAutoPtr<IShader> pBasicMeshVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Basic Mesh VS";
                ShaderCI.FilePath = "VTFS/BasicMesh.vsh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBasicMeshVS);
        }
        RefCntAutoPtr<IShader> pBasicMeshOpaqueDepthPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Basic Mesh Opaque Depth PS";
                ShaderCI.FilePath = "VTFS/BasicMeshOpaqueDepth.psh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBasicMeshOpaqueDepthPS);
        }
        RefCntAutoPtr<IShader> pBasicMeshClusterSamplesPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Basic Mesh Cluster Samples PS";
                ShaderCI.FilePath = "VTFS/BasicMeshClusterSamples.psh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBasicMeshClusterSamplesPS);
        }
        RefCntAutoPtr<IShader> pBasicMeshOpaqueLightingPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Basic Mesh Opaque Lighting PS";
                ShaderCI.FilePath = "VTFS/BasicMeshOpaqueLighting.psh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pBasicMeshOpaqueLightingPS);
        }

        {
                PSODesc.Name = "Basic Model Opaque Depth PSO";
                PSOCreateInfo.pVS = pBasicMeshPosOnlyVS;
                PSOCreateInfo.pPS = pBasicMeshOpaqueDepthPS;
                PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_R32_FLOAT;
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_VERTEX, "_BasicModelCameraCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);
                // PSOCreateInfo.GraphicsPipeline.DSVFormat = DepthBufferFormat;
                Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pOpaqueDepthPSO);
                LD_VERIFY(m_pOpaqueDepthPSO, "Failed to create {}", PSODesc.Name);

                m_BasicModelCameraCBAttributes.AddMat4("ModelViewProjection");
                m_BasicModelCameraCBAttributes.AddMat4("ModelView");
                m_BasicModelCameraCBAttributes.AddMat4("Model");
                m_BasicModelCameraCBData = m_BasicModelCameraCBAttributes.CreateData("Basic Model Camera CB Data");

                CreateUniformBuffer(Renderer::GetDevice(), m_BasicModelCameraCBAttributes.GetSize(), "_BasicModelCameraCB", &m_BasicModelCameraCB);
                StateTransitionDesc Barriers[] =
                {
                        {m_BasicModelCameraCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
                m_pOpaqueDepthPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "_BasicModelCameraCB")->Set(m_BasicModelCameraCB);
                Assets::GetShaderLibrary().static_buffers["_BasicModelCameraCB"] = m_BasicModelCameraCB;
        }
        {
                PSODesc.Name = "Cluster Samples PSO";
                PSOCreateInfo.pVS = pBasicMeshPosOnlyVS;
                PSOCreateInfo.pPS = pBasicMeshClusterSamplesPS;
                PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 0;
                PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_UNKNOWN;
                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = False;
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc. = False;
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_VERTEX, "_BasicModelCameraCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_PIXEL, "_ClusterCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_PIXEL, "RWClusterFlags", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pClusterSamplesPSO);
                LD_VERIFY(m_pClusterSamplesPSO, "Failed to create {}", PSODesc.Name);
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = True;
                PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
                m_pClusterSamplesPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "_BasicModelCameraCB")->Set(m_BasicModelCameraCB);
                m_pClusterSamplesPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "_ClusterCB")->Set(m_ClusterCB);
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
        }
        {
                PSODesc.Name = "Basic Mesh Opaque Lighting PSO";
                PSOCreateInfo.pVS = pBasicMeshVS;
                PSOCreateInfo.pPS = pBasicMeshOpaqueLightingPS;
                PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
                PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Renderer::GetDefaultRTVFormat();
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = True;
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_VERTEX, "_BasicModelCameraCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_PIXEL, "_ClusterCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_PIXEL, "_CameraCB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                        {SHADER_TYPE_PIXEL, "PointLightGrid", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_PIXEL, "PointLightIndexList", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                        {SHADER_TYPE_PIXEL, "PointLights", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);

                Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pBasicMeshOpaqueLightingPSO);
                LD_VERIFY(m_pBasicMeshOpaqueLightingPSO, "Failed to create {}", PSODesc.Name);
                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = True;
                PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
                m_pBasicMeshOpaqueLightingPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "_BasicModelCameraCB")->Set(m_BasicModelCameraCB);
                m_pBasicMeshOpaqueLightingPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "_ClusterCB")->Set(m_ClusterCB);
                m_pBasicMeshOpaqueLightingPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "_CameraCB")->Set(m_CameraCB);
        }
}

void VTFSRenderer::CreateWholeScreenPSOs()
{
        LD_PROFILE_FUNCTION();
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Renderer::GetDefaultRTVFormat();
        PSOCreateInfo.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT_S8X24_UINT;
        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        SamplerDesc SamLinearClampDesc
        {
                FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
                TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };

        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();

        RefCntAutoPtr<IShader> pWholeScreenVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Whole Screen VS";
                ShaderCI.FilePath = "VTFS/WholeScreen.vsh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pWholeScreenVS);
        }

        RefCntAutoPtr<IShader> pDebugDepthPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Debug Depth PS";
                ShaderCI.FilePath = "VTFS/Debug/ViewDepth.psh";
                Renderer::GetDevice()->CreateShader(ShaderCI, &pDebugDepthPS);
        }

        {
                PSODesc.Name = "Depth Debug PSO";
                PSOCreateInfo.pVS = pWholeScreenVS;
                PSOCreateInfo.pPS = pDebugDepthPS;
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = false;
                // PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.StencilEnable = false;
                ShaderResourceVariableDesc Vars[] =
                {
                        {SHADER_TYPE_PIXEL, "g_SubpassInputDepthZ", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
                };
                PSODesc.ResourceLayout.Variables = Vars;
                PSODesc.ResourceLayout.NumVariables = _countof(Vars);
                Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pDebugDepthPSO);
                LD_VERIFY(m_pDebugDepthPSO, "Failed to create {}", PSODesc.Name);
        }
}

void VTFSRenderer::ReleaseAllRenderTargetResources()
{
        LD_PROFILE_FUNCTION();
        m_PerRenderTargetCache.clear();
}
}
