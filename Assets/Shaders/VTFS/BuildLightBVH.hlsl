#include "VTFS/Common.hlsl"

#ifndef NUM_THREADS
#define NUM_THREADS 512 //32 * 16
#endif

#define FLT_MAX 3.402823466e+38

// The number of nodes in each level of the BVH
static const uint NumLevelNodes[] = {
    1,          // Level 0 ( 32^0 )
    32,         // Level 1 ( 32^1 )
    1024,       // Level 2 ( 32^2 )
    32768,      // Level 3 ( 32^3 )
    1048576,    // Level 4 ( 32^4 )
    33554432,   // Level 5 ( 32^5 )
    1073741824, // Level 6 ( 32^6 )
};

// The first index of a node in the BVH 
// given the level of the BVH.
static const uint FirstNodeIndex[] = {
    0,          // Level 0
    1,          // Level 1
    33,         // Level 2
    1057,       // Level 3
    33825,      // Level 4
    1082401,    // Level 5
    34636833,   // Level 6
};

groupshared float4 gs_AABBMin[NUM_THREADS];
groupshared float4 gs_AABBMax[NUM_THREADS];

void LogStepReduction( ComputeShaderInput CSIn )
{
    uint reduceIndex = 32 >> 1;
    uint mod32GroupIndex = CSIn.groupIndex % 32;

    while ( mod32GroupIndex < reduceIndex )
    {
        gs_AABBMin[CSIn.groupIndex] = min( gs_AABBMin[CSIn.groupIndex], gs_AABBMin[CSIn.groupIndex + reduceIndex] );
        gs_AABBMax[CSIn.groupIndex] = max( gs_AABBMax[CSIn.groupIndex], gs_AABBMax[CSIn.groupIndex + reduceIndex] );

        reduceIndex >>= 1;
    }
}

[numthreads(NUM_THREADS,1,1)]
void BuildBottom(ComputeShaderInput CSIn)
{
    uint light_index;
    uint num_levels, node_index, node_offset;
    float4 aabb_min, aabb_max;

    uint leaf_index = CSIn.dispatchThreadID.x;

    if (leaf_index < LightCountsCB.NumPointLights)
    {
        light_index = PointLightIndicies[leaf_index];
        PointLight point_light = PointLights[light_index];
        aabb_min = point_light.PositionVS - point_light.Range;
        aabb_max = point_light.PositionVS + point_light.Range;
    }
    else
    {
        aabb_min = float4(FLT_MAX, FLT_MAX, FLT_MAX, 1);
        aabb_max = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);
    }

    gs_AABBMin[CSIn.groupIndex] = aabb_min;
    gs_AABBMax[CSIn.groupIndex] = aabb_max;

    LogStepReduction(CSIn);

    if (CSIn.dispatchThreadID.x % 32 == 0)
    {
        num_levels = BVHParamsCB.PointLightLevels;
        node_offset = CSIn.dispatchThreadID.x / 32;

        if (num_levels > 0 && node_offset < NumLevelNodes[num_levels - 1])
        {
            node_index = FirstNodeIndex[num_levels - 1] + node_offset;
            RWPointLightBVH[node_index].Min = gs_AABBMin[CSIn.groupIndex];
            RWPointLightBVH[node_index].Max = gs_AABBMax[CSIn.groupIndex];
        }
    }

    if (leaf_index < LightCountsCB.NumSpotLights)
    {
        light_index = SpotLightIndicies[leaf_index];
        SpotLight spot_light = SpotLights[light_index];
        aabb_min = spot_light.PositionVS - spot_light.Range;
        aabb_max = spot_light.PositionVS + spot_light.Range;
    }
    else
    {
        aabb_min = float4(FLT_MAX, FLT_MAX, FLT_MAX, 1);
        aabb_max = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);
    }

    aabb_min += LightCountsCB.NumSpotLights;

    gs_AABBMin[CSIn.groupIndex] = aabb_min;
    gs_AABBMax[CSIn.groupIndex] = aabb_max;

    LogStepReduction(CSIn);

    if (CSIn.dispatchThreadID.x % 32 == 0)
    {
        num_levels = BVHParamsCB.SpotLightLevels;
        node_offset = CSIn.dispatchThreadID.x / 32;

        if (num_levels > 0 && node_offset < NumLevelNodes[num_levels - 1])
        {
            node_index = FirstNodeIndex[num_levels - 1] + node_offset;
            RWSpotLightBVH[node_index].Min = gs_AABBMin[CSIn.groupIndex];
            RWSpotLightBVH[node_index].Max = gs_AABBMax[CSIn.groupIndex];
        }
    }
}


[numthreads(NUM_THREADS,1,1)]
void BuildTop(ComputeShaderInput CSIn)
{
    uint num_levels, child_level;
    uint child_offset, child_index;
    uint node_offset, node_index;
    float4 aabb_min, aabb_max;

    child_level = BVHParamsCB.ChildLevel;
    num_levels = BVHParamsCB.PointLightLevels;
    child_offset = CSIn.dispatchThreadID.x;

    if (child_level < num_levels && child_offset < NumLevelNodes[BVHParamsCB.ChildLevel])
    {
        child_index = FirstNodeIndex[child_level] + child_offset;

        aabb_min = RWPointLightBVH[child_index].Min;
        aabb_max = RWPointLightBVH[child_index].Max;
    }
    else
    {
        aabb_min = float4(FLT_MAX, FLT_MAX, FLT_MAX, 1);
        aabb_max = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);
    }

    gs_AABBMin[CSIn.groupIndex] = aabb_min;
    gs_AABBMax[CSIn.groupIndex] = aabb_max;

    LogStepReduction(CSIn);

    if (CSIn.dispatchThreadID.x % 32 == 0)
    {
        node_offset = CSIn.dispatchThreadID / 32;
        if (child_level < num_levels && node_offset < NumLevelNodes[BVHParamsCB.ChildLevel - 1])
        {
            node_index = FirstNodeIndex[child_level - 1] + node_offset;
            RWPointLightBVH[node_index].Min = gs_AABBMin[CSIn.groupIndex];
            RWPointLightBVH[node_index].Max = gs_AABBMax[CSIn.groupIndex];
        }
    }

    num_levels = BVHParamsCB.SpotLightLevels;

    if (child_level < num_levels && child_offset < NumLevelNodes[BVHParamsCB.ChildLevel])
    {
        child_index = FirstNodeIndex[child_level] + child_offset;

        aabb_min = RWSpotLightBVH[child_index].Min;
        aabb_max = RWSpotLightBVH[child_index].Max;
    }
    else
    {
        aabb_min = float4(FLT_MAX, FLT_MAX, FLT_MAX, 1);
        aabb_max = float4(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);
    }

    gs_AABBMin[CSIn.groupIndex] = aabb_min;
    gs_AABBMax[CSIn.groupIndex] = aabb_max;

    LogStepReduction(CSIn);

    if (CSIn.dispatchThreadID.x % 32 == 0)
    {
        node_offset = CSIn.dispatchThreadID / 32;
        if (child_level < num_levels && node_offset < NumLevelNodes[BVHParamsCB.ChildLevel - 1])
        {
            node_index = FirstNodeIndex[child_level - 1] + node_offset;
            RWSpotLightBVH[node_index].Min = gs_AABBMin[CSIn.groupIndex];
            RWSpotLightBVH[node_index].Max = gs_AABBMax[CSIn.groupIndex];
        }
    }
}