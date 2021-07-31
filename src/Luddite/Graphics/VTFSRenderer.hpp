#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#include "Luddite/Core/Assets.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Graphics/StreamingBuffer.hpp"
#include "Luddite/Graphics/Lights.hpp"

namespace Luddite
{
class LUDDITE_API Renderer;
class LUDDITE_API RenderScene;
struct DispatchIndirectArgument
{
        glm::uvec3 thread_groups;
};
class LUDDITE_API VTFSRenderer
{
        public:
        void Initialize(Diligent::TEXTURE_FORMAT RTVFormat);
        void ReleaseAllRenderTargetResources();
        void Render(const RenderTarget& render_target, const Camera& camera, const RenderScene& render_scene);

        inline void DebugFreezeClusters() {m_FreezeClusters = true;}
        inline void DebugUnfreezeClusters() {m_FreezeClusters = false;}

        private:
        struct PerRenderTargetData
        {
                Diligent::RefCntAutoPtr<Diligent::IFramebuffer> pFrameBuffer;
                Diligent::RefCntAutoPtr<Diligent::ITexture> pDepthZTexture;

                glm::uvec3 cluster_dimensions;
                //SRBs
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> ComputeClusterGridSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> OpaqueDepthSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> ClusterSamplesSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> FindUniqueClustersSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> UpdateIndirectArgumentBuffersSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> UpdateLightsSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> ReduceLightsAABB1SRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> ReduceLightsAABB2SRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> ComputeLightMortonCodesSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> RadixSortPointLightsSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> RadixSortSpotLightsSRB;

                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergePathPartitionsPointLightsSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergePathPartitionsSpotLightsSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergeSortPointLightsSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergeSortSpotLightsSRB;

                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergePathPartitionsPointLightsSwappedSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergePathPartitionsSpotLightsSwappedSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergeSortPointLightsSwappedSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> MergeSortSpotLightsSwappedSRB;

                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> BuildBVHBottomSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> BuildBVHTopSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> AssignLightsToClustersSRB;
                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> AssignLightsToClustersBruteForceSRB;

                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> BasicMeshOpaqueLightingSRB;

                Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> DebugDepthSRB;

                //DepthOnly Render Target




                //PointLightIndexCounter
                Diligent::RefCntAutoPtr<Diligent::IBuffer> PointLightIndexCounterBuffer;
                //SpotLightIndexCounter
                Diligent::RefCntAutoPtr<Diligent::IBuffer> SpotLightIndexCounterBuffer;
                //PointLightIndexList
                Diligent::RefCntAutoPtr<Diligent::IBuffer> PointLightIndexListBuffer;
                //SpotLightIndexList
                Diligent::RefCntAutoPtr<Diligent::IBuffer> SpotLightIndexListBuffer;
                //PointLightGrid
                Diligent::RefCntAutoPtr<Diligent::IBuffer> PointLightGridBuffer;
                //SpotLightGrid
                Diligent::RefCntAutoPtr<Diligent::IBuffer> SpotLightGridBuffer;

                //ClusterFlags
                Diligent::RefCntAutoPtr<Diligent::IBuffer> ClusterFlagsBuffer;
                //UniqueClusters
                Diligent::RefCntAutoPtr<Diligent::IBuffer> UniqueClustersBuffer;
                Diligent::RefCntAutoPtr<Diligent::IBuffer> UniqueClustersCounterBuffer;
                //AssignLightsToClustersArgumentBuffer
                Diligent::RefCntAutoPtr<Diligent::IBuffer> AssignLightsToClustersArgumentBufferBuffer;

                //ClusterAABBs
                Diligent::RefCntAutoPtr<Diligent::IBuffer> ClusterAABBsBuffer;

                glm::mat4 prev_projection_matrix;

                //LightCullingDebugTexture
        };
        PerRenderTargetData CreateRenderTargetData(const RenderTarget& render_target, const Camera& camera);
        void ComputeClusterGrid(PerRenderTargetData& data, const RenderTarget& render_target, const Camera& camera);
        bool MergeSort(Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergePathPartitionsSRB,
                       Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergePathPartitionsSwappedSRB,
                       Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergeSortSRB,
                       Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pMergeSortSwappedSRB,
                       uint32_t total_values, uint32_t chunk_size);
        PerRenderTargetData* GetRenderTargetData(const RenderTarget& render_target, const Camera& camera);
        std::unordered_map<Diligent::ITextureView*, PerRenderTargetData> m_PerRenderTargetCache;

        bool m_FreezeClusters = false;


        void CreateLightBuffers();
        void CreateComputePSOs();
        void CreateDrawPSOs();
        void CreateWholeScreenPSOs();
        int m_ClusterGridBlockSize = 64;
        int m_MaxAvgLightsPerTile = 128;

        //Render Pass
        Diligent::RefCntAutoPtr<Diligent::IRenderPass> m_pRenderPass;

        //      PSOs:

        // Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pComputeGridFrustumsPSO;
        // ShaderAttributeListDescription m_ComputeGridFrustumsDesc;
        // ShaderAttributeListData m_ComputeGridFrustumsData;

        //ComputeClusterGrid
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pComputeClusterGridPSO;
        ShaderAttributeListDescription m_ClusterCBAttributes;
        ShaderAttributeListData m_ClusterCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_ClusterCB;

        //DepthPrePass
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pOpaqueDepthPSO;
        ShaderAttributeListDescription m_BasicModelCameraCBAttributes;
        ShaderAttributeListData m_BasicModelCameraCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_BasicModelCameraCB;

        //Cluster Samples
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pClusterSamplesPSO;

        //Find Unique Clusters
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pFindUniqueClustersPSO;

        //Update Indirect Argument Buffers
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pUpdateIndirectArgumentBuffersPSO;

        //Update Lights
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pUpdateLightsPSO;
        ShaderAttributeListDescription m_UpdateLightsCBAttributes;
        ShaderAttributeListData m_UpdateLightsCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_UpdateLightsCB;
        ShaderAttributeListDescription m_LightCountsCBAttributes;
        ShaderAttributeListData m_LightCountsCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_LightCountsCB;

        //ReduceLightsAABB1
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pReduceLightsAABB1PSO;
        ShaderAttributeListDescription m_DispatchParamsCBAttributes;
        ShaderAttributeListData m_DispatchParamsCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_DispatchParamsCB;

        //ReduceLightsAABB2
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pReduceLightsAABB2PSO;
        ShaderAttributeListDescription m_ReductionParamsCBAttributes;
        ShaderAttributeListData m_ReductionParamsCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_ReductionParamsCB;

        //ComputeLightMortonCodes
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pComputeLightMortonCodesPSO;

        //RadixSort
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pRadixSortPSO;
        ShaderAttributeListDescription m_SortParamsCBAttributes;
        ShaderAttributeListData m_SortParamsCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_SortParamsCB;

        //MergePathPartitions
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pMergePathPartitionsPSO;

        //MergeSort
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pMergeSortPSO;

        //BuildBVHBottom
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pBuildBVHBottomPSO;
        ShaderAttributeListDescription m_BVHParamsCBAttributes;
        ShaderAttributeListData m_BVHParamsCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_BVHParamsCB;

        //BuildBVHTop
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pBuildBVHTopPSO;

        //Assign Lights to Clusters
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pAssignLightsToClustersPSO;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pAssignLightsToClustersBruteForcePSO;

        //Opaque
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pOpaquePSO;

        //Transparent
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pTransparentPSO;

        //DebugPointLights
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pDebugPointLightsPSO;

        //DebugSpotLights
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pDebugSpotLightsPSO;

        //DebugDepth
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pDebugDepthPSO;

        //Lighting
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pBasicMeshOpaqueLightingPSO;

        static constexpr uint32_t SORT_NUM_THREADS_PER_THREAD_GROUP = 256;
        static constexpr uint32_t SORT_ELEMENTS_PER_THREAD = 8;
        static constexpr uint32_t BVH_NUM_THREADS = 32 * 16;

        //PointLightsBuffer
        std::unique_ptr<StreamingBuffer<PointLightGPU> > m_pPointLightsBuffer;
        //SpotLightsBuffer
        std::unique_ptr<StreamingBuffer<SpotLightGPU> > m_pSpotLightsBuffer;
        //DirectionalLightsBuffer
        std::unique_ptr<StreamingBuffer<DirectionalLightGPU> > m_pDirectionalLightsBuffer;

        //LightsAABBs
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pLightsAABBBuffer;

        //PointLightMortonCodes
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pPointLightMortonCodesBuffer;
        //PointLightIndicies
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pPointLightMortonCodes_OUTBuffer;
        //PointLightIndicies_OUT
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pPointLightIndiciesBuffer;
        //PointLightMortonCodes_OUT
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pPointLightIndicies_OUTBuffer;
        //PointLightBVH
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pPointLightBVHBuffer;

        //SpotLightMortonCodes
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pSpotLightMortonCodesBuffer;
        //SpotLightIndicies
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pSpotLightMortonCodes_OUTBuffer;
        //SpotLightIndicies_OUT
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pSpotLightIndiciesBuffer;
        //SpotLightMortonCodes_OUT
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pSpotLightIndicies_OUTBuffer;
        //SpotLightBVH
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pSpotLightBVHBuffer;

        //MergePathPartitions
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pMergePathPartitionsBuffer;

        static constexpr uint32_t AVERAGE_OVERLAPPING_LIGHTS_PER_CLUSTER = 100;
        static constexpr uint32_t MAX_POINT_LIGHTS = 5000;
        static constexpr uint32_t MAX_SPOT_LIGHTS = 5000;
        static constexpr uint32_t MAX_DIRECTIONAL_LIGHTS = 100;

        #ifdef LD_PLATFORM_DESKTOP
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        // static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT_S8X24_UINT;
        #endif

        #ifdef LD_PLATFORM_MOBILE
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D16_UNORM;
        #endif

        //Compute

        ShaderAttributeListDescription m_CameraCBAttributes;
        ShaderAttributeListData m_CameraCBData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_CameraCB;
};

struct GridFrustumPlane
{
        glm::vec3 N;
        float D;
};

struct GridFrustumFrustum
{
        GridFrustumPlane planes[4];
};

struct AABB
{
        glm::vec4 min;
        glm::vec4 max;
};
}