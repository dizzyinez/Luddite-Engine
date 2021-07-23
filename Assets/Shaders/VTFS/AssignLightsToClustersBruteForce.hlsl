#include "VTFS/Common.hlsl"

#define NUM_THREADS 1024

groupshared uint gs_ClusterIndex1D;
groupshared AABB gs_ClusterAABB;

groupshared uint gs_PointLightCount;
groupshared uint gs_SpotLightCount;
groupshared uint gs_PointLightStartOffset;
groupshared uint gs_SpotLightStartOffset;
groupshared uint gs_PointLightList[1024];
groupshared uint gs_SpotLightList[1024];

#define AppendLight( lightIndex, counter, lightList ) \
    InterlockedAdd( counter, 1, index ); \
    if ( index < 1024 ) \
    { \
        lightList[index] = lightIndex; \
    }

[numthreads( NUM_THREADS, 1, 1 )]
void main( ComputeShaderInput CSIn )
{
    uint i, index;

    if (CSIn.groupIndex == 0)
    {
        gs_PointLightCount = 0;
        gs_SpotLightCount = 0;

        gs_ClusterIndex1D = UniqueClusters[CSIn.groupID.x];
        gs_ClusterAABB = ClusterAABBs[gs_ClusterIndex1D];
    }

    GroupMemoryBarrierWithGroupSync();

    for (i = CSIn.groupIndex; i < LightCountsCB.NumPointLights; i+= NUM_THREADS)
    {
        if (PointLights[i].Enabled)
        {
            PointLight point_light = PointLights[i];
            Sphere sphere = {point_light.PositionVS.xyz, point_light.Range};

            if (SphereInsideAABB(sphere, gs_ClusterAABB))
            {
                AppendLight(i, gs_PointLightCount, gs_PointLightList);
            }
        }
    }

    for (i = CSIn.groupIndex; i < LightCountsCB.NumSpotLights; i+= NUM_THREADS)
    {
        if (SpotLights[i].Enabled)
        {
            SpotLight spot_light = SpotLights[i];
            Sphere sphere = {spot_light.PositionVS.xyz, spot_light.Range};

            if (SphereInsideAABB(sphere, gs_ClusterAABB))
            {
                AppendLight(i, gs_SpotLightCount, gs_SpotLightList);
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    if (CSIn.groupIndex == 0)
    {
        InterlockedAdd(RWPointLightIndexCounter[0], gs_PointLightCount, gs_PointLightStartOffset);
        RWPointLightGrid[gs_ClusterIndex1D] = uint2(gs_PointLightStartOffset, gs_PointLightCount);

        InterlockedAdd(RWSpotLightIndexCounter[0], gs_SpotLightCount, gs_SpotLightStartOffset);
        RWSpotLightGrid[gs_ClusterIndex1D] = uint2(gs_SpotLightStartOffset, gs_SpotLightCount);
    }

    GroupMemoryBarrierWithGroupSync();

    for (i = CSIn.groupIndex; i < gs_PointLightCount; i += NUM_THREADS)
    {
        RWPointLightIndexList[gs_PointLightStartOffset + i] = gs_PointLightList[i];
    }

    for (i = CSIn.groupIndex; i < gs_SpotLightCount; i+= NUM_THREADS)
    {
        RWSpotLightIndexList[gs_SpotLightStartOffset + i] = gs_SpotLightList[i];
    }
}