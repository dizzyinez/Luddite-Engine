struct ComputeShaderInput
{
    uint3 groupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

struct CameraData
{
    float4x4 Projection;
    float4x4 InverseProjection;
    float2 ViewDimensions;
};

cbuffer _CameraCB : register(b0)
{
    CameraData CameraCB;
};

struct BasicModelCameraData
{ 
    float4x4 ModelViewProjection;
    float4x4 ModelView;
    float4x4 Model;
};

cbuffer _BasicModelCameraCB : register(b1)
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

cbuffer _ClusterCB : register(b2)
{
    ClusterData ClusterCB;
};

struct LightCountsData
{
    uint NumPointLights;
    uint NumSpotLights;
    uint NumDirectionalLights;
};

cbuffer _LightCountsCB : register(b3)
{
    LightCountsData LightCountsCB;
};

struct DispatchParamsData
{
    uint3 NumThreadGroups;
    uint3 NumThreads;
};

cbuffer _DispatchParamsCB : register(b4)
{
    DispatchParamsData DispatchParamsCB;
};

struct ReductionParamsData
{
    uint NumElements;
};

cbuffer _ReductionParamsCB : register(b5)
{
    ReductionParamsData ReductionParamsCB;
};

struct SortParamsData
{
    uint NumElements;
    uint ChunkSize;
};

cbuffer _SortParamsCB : register(b6)
{
    SortParamsData SortParamsCB;
};

struct BVHParamsData
{
    uint PointLightLevels;
    uint SpotLightLevels;
    uint ChildLevel;
};

cbuffer _BVHParamsCB : register(b7)
{
    BVHParamsData BVHParamsCB;
};
StructuredBuffer<PointLight> PointLights : register(t0);
StructuredBuffer<SpotLight> SpotLights : register(t1);
StructuredBuffer<DirectionalLight> DirectionalLights : register(t2);

StructuredBuffer<uint> PointLightIndexList : register(t3);
StructuredBuffer<uint> SpotLightIndexList : register(t4);
StructuredBuffer<uint2> PointLightGrid : register(t5);
StructuredBuffer<uint2> SpotLightGrid : register(t6);

StructuredBuffer<uint> UniqueClusters : register(t7);
StructuredBuffer<AABB> ClusterAABBs : register(t8);

StructuredBuffer<AABB> LightsAABB : register(t9);

StructuredBuffer<uint> InputKeys : register(t10);
StructuredBuffer<uint> InputValues : register(t11);
StructuredBuffer<uint> MergePathPartitions : register(t12);

StructuredBuffer<uint> PointLightIndicies : register(t13);
StructuredBuffer<uint> SpotLightIndicies : register(t14);

StructuredBuffer<AABB> PointLightBVH : register(t15);
StructuredBuffer<AABB> SpotLightBVH : register(t16);

StructuredBuffer<bool> ClusterFlags : register(t17);


// RWStructuredBuffer<uint> RWUniqueClusters;
RWStructuredBuffer<PointLight> RWPointLights : register(u0);
RWStructuredBuffer<SpotLight> RWSpotLights : register(u1);
RWStructuredBuffer<DirectionalLight> RWDirectionalLights : register(u2);

RWStructuredBuffer<uint> RWPointLightIndexList : register(u3);
RWStructuredBuffer<uint> RWSpotLightIndexList : register(u4);
RWStructuredBuffer<uint2> RWPointLightGrid : register(u5);
RWStructuredBuffer<uint2> RWSpotLightGrid : register(u6);

AppendStructuredBuffer<uint> RWUniqueClusters : register(u7);
RWStructuredBuffer<AABB> RWClusterAABBs : register(u8);

RWStructuredBuffer<AABB> RWLightsAABB : register(u9);

RWStructuredBuffer<uint> OutputKeys : register(u10);
RWStructuredBuffer<uint> OutputValues : register(u11);
RWStructuredBuffer<uint> RWMergePathPartitions : register(u12);

RWStructuredBuffer<uint> RWPointLightMortonCodes : register(u13);
RWStructuredBuffer<uint> RWSpotLightMortonCodes : register(u14);
RWStructuredBuffer<uint> RWPointLightIndicies : register(u15);
RWStructuredBuffer<uint> RWSpotLightIndicies : register(u16);

RWStructuredBuffer<AABB> RWPointLightBVH : register(u17);
RWStructuredBuffer<AABB> RWSpotLightBVH : register(u18);

RWStructuredBuffer<uint> RWPointLightIndexCounter : register(u19);
RWStructuredBuffer<uint> RWSpotLightIndexCounter : register(u20);

RWStructuredBuffer<bool> RWClusterFlags : register(u21);