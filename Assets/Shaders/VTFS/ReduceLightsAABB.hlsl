#include "VTFS/Common.hlsl"

/**
 * A compute shader to determine the view space AABB of all of the lights in the scene.
 * A parallel reduction algorithm is used to reduce the light AABBs to a single AABB that
 * encompasses all lights.
 * Source: The CUDA Handbook (2013), Nicholas Wilt.
 */
 
#ifndef NUM_THREADS
#define NUM_THREADS 512
#endif

#define FLT_MAX 3.402823466e+38F

groupshared float4 gs_AABBMin[NUM_THREADS];
groupshared float4 gs_AABBMax[NUM_THREADS];

void LogStepReduction(ComputeShaderInput CSIn)
{
    uint reduce_index = NUM_THREADS >> 1;

    [unroll]
    while (reduce_index > 32)
    {
        if (CSIn.groupIndex < reduce_index)
        {
            gs_AABBMin[CSIn.groupIndex] = min(gs_AABBMin[CSIn.groupIndex], gs_AABBMin[CSIn.groupIndex + reduce_index]);
            gs_AABBMax[CSIn.groupIndex] = max(gs_AABBMax[CSIn.groupIndex], gs_AABBMax[CSIn.groupIndex + reduce_index]);
        }
        GroupMemoryBarrierWithGroupSync();
        reduce_index >>= 1;
    }

    if (CSIn.groupIndex < 32)
    {
        [unroll]
        while (reduce_index > 0)
        {
            if (NUM_THREADS >= reduce_index << 1)
            {
            gs_AABBMin[CSIn.groupIndex] = min(gs_AABBMin[CSIn.groupIndex], gs_AABBMin[CSIn.groupIndex + reduce_index]);
            gs_AABBMax[CSIn.groupIndex] = max(gs_AABBMax[CSIn.groupIndex], gs_AABBMax[CSIn.groupIndex + reduce_index]);
            }
            reduce_index >>= 1;
        }
        if (CSIn.groupIndex == 0)
        {
            RWLightsAABB[CSIn.groupID.x].Min = gs_AABBMin[CSIn.groupIndex];
            RWLightsAABB[CSIn.groupID.x].Max = gs_AABBMax[CSIn.groupIndex];
        }
    }
}

[numthreads(NUM_THREADS,1,1)]
void reduce1(ComputeShaderInput CSIn)
{
    uint i;
    float4 aabbMin = float4(FLT_MAX, FLT_MAX, FLT_MAX, 1);
    float4 aabbMax = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);

    for (i = CSIn.dispatchThreadID.x; i < LightCountsCB.NumPointLights; i+= NUM_THREADS * DispatchParamsCB.NumThreadGroups.x)
    {
        PointLight point_light = PointLights[i];

        aabbMin = min(aabbMin, point_light.PositionVS - point_light.Range);
        aabbMax = max(aabbMax, point_light.PositionVS + point_light.Range);
    }

    for (i = CSIn.dispatchThreadID.x; i < LightCountsCB.NumSpotLights; i+= NUM_THREADS * DispatchParamsCB.NumThreadGroups.x)
    {
        SpotLight spot_light = SpotLights[i];

        aabbMin = min(aabbMin, spot_light.PositionVS - spot_light.Range);
        aabbMax = max(aabbMax, spot_light.PositionVS + spot_light.Range);
    }
    
    gs_AABBMin[CSIn.groupIndex] = aabbMin;
    gs_AABBMax[CSIn.groupIndex] = aabbMax;

    GroupMemoryBarrierWithGroupSync();

    LogStepReduction(CSIn);
}

[numthreads(NUM_THREADS,1,1)]
void reduce2(ComputeShaderInput CSIn)
{
    uint i;
    float4 aabbMin = float4(FLT_MAX, FLT_MAX, FLT_MAX, 1);
    float4 aabbMax = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);

    for (i = CSIn.groupIndex; i < ReductionParamsCB.NumElements; i += NUM_THREADS * DispatchParamsCB.NumThreadGroups.x)
    {
        aabbMin = min(aabbMin, RWLightsAABB[i].Min);
        aabbMax = max(aabbMax, RWLightsAABB[i].Max);
    }

    gs_AABBMin[CSIn.groupIndex] = aabbMin;
    gs_AABBMax[CSIn.groupIndex] = aabbMax;

    GroupMemoryBarrierWithGroupSync();

    LogStepReduction(CSIn);
}


