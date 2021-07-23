struct ComputeShaderInput
{
    uint2 groupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint2 groupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint2 dispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

struct CameraData
{
    float4x4 Projection;
    float4x4 InverseProjection;
    float2 ViewDimensions;
};

cbuffer _CameraCB
{
    CameraData CameraCB;
};

struct BasicModelCameraData
{ 
    float4x4 ModelViewProjection;
    float4x4 ModelView;
    float4x4 Model;
};

cbuffer _BasicModelCameraCB
{
    BasicModelCameraData BasicModelCameraCB;
};

struct ClusterData
{
    uint3 grid_dim;
    float view_near;
    uint2 size;
    float near_k;
    float log_grid_dim_y;
};

cbuffer _ClusterCB
{
    ClusterData ClusterCB;
};

struct LightCountsData
{
    uint NumPointLights;
    uint NumSpotLights;
    uint NumDirectionalLights;
};

cbuffer _LightCountsCB
{
    LightCountsData LightCountsCB;
};

struct DispatchParamsData
{
    uint3 NumThreadGroups;
    uint3 NumThreads;
};

cbuffer _DispatchParamsCB
{
    DispatchParamsData DispatchParamsCB;
};

struct ReductionParamsData
{
    uint NumElements;
};

cbuffer _ReductionParamsCB
{
    ReductionParamsData ReductionParamsCB;
};

struct SortParamsData
{
    uint NumElements;
    uint ChunkSize;
};

cbuffer _SortParamsCB
{
    SortParamsData SortParamsCB;
};

struct BVHParamsData
{
    uint PointLightLevels;
    uint SpotLightLevels;
    uint ChildLevel;
};

cbuffer _BVHParamsCB
{
    BVHParamsData BVHParamsCB;
};

RWStructuredBuffer<AABB> RWClusterAABBs;
StructuredBuffer<AABB> ClusterAABBs;

RWStructuredBuffer<bool> RWClusterFlags;
StructuredBuffer<bool> ClusterFlags;

RWStructuredBuffer<uint> RWUniqueClusters;
StructuredBuffer<uint> UniqueClusters;

RWStructuredBuffer<PointLight> RWPointLights;
StructuredBuffer<PointLight> PointLights;

RWStructuredBuffer<SpotLight> RWSpotLights;
StructuredBuffer<SpotLight> SpotLights;

RWStructuredBuffer<DirectionalLight> RWDirectionalLights;
StructuredBuffer<DirectionalLight> DirectionalLights;

RWStructuredBuffer<AABB> RWLightsAABB;
StructuredBuffer<AABB> LightsAABB;

RWStructuredBuffer<uint> RWPointLightMortonCodes;
RWStructuredBuffer<uint> RWSpotLightMortonCodes;
RWStructuredBuffer<uint> RWPointLightIndicies;
StructuredBuffer<uint> PointLightIndicies;
RWStructuredBuffer<uint> RWSpotLightIndicies;
StructuredBuffer<uint> SpotLightIndicies;

StructuredBuffer<uint> InputKeys;
StructuredBuffer<uint> InputValues;
RWStructuredBuffer<uint> OutputKeys;
RWStructuredBuffer<uint> OutputValues;

RWStructuredBuffer<uint> RWMergePathPartitions;
StructuredBuffer<uint> MergePathPartitions;

RWStructuredBuffer<AABB> RWPointLightBVH;
StructuredBuffer<AABB> PointLightBVH;

RWStructuredBuffer<AABB> RWSpotLightBVH;
StructuredBuffer<AABB> SpotLightBVH;

RWStructuredBuffer<uint> RWPointLightIndexCounter;

RWStructuredBuffer<uint> RWPointLightIndexList;
StructuredBuffer<uint> PointLightIndexList;

RWStructuredBuffer<uint2> RWPointLightGrid;
StructuredBuffer<uint2> PointLightGrid;

RWStructuredBuffer<uint> RWSpotLightIndexCounter;

RWStructuredBuffer<uint> RWSpotLightIndexList;
StructuredBuffer<uint> SpotLightIndexList;

RWStructuredBuffer<uint2> RWSpotLightGrid;
StructuredBuffer<uint2> SpotLightGrid;