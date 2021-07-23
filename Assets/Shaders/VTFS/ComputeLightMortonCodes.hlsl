#include "VTFS/Common.hlsl"

#ifndef NUM_THREADS
#define NUM_THREADS 1024
#endif

groupshared AABB gs_AABB;
groupshared float4 gs_AABBRange;

// Produce a 3k-bit morton code from a quantized coordinate.
uint MortonCode( uint3 quantizedCoord, uint k )
{
    uint mortonCode = 0;
    uint bitMask = 1;
    uint bitShift = 0;
    uint kBits = ( 1 << k );

    while ( bitMask < kBits )
    {
        // Interleave the bits of the X, Y, and Z coordinates to produce the final Morton code.
        mortonCode |= ( quantizedCoord.x & bitMask ) << ( bitShift + 0 );
        mortonCode |= ( quantizedCoord.y & bitMask ) << ( bitShift + 1 );
        mortonCode |= ( quantizedCoord.z & bitMask ) << ( bitShift + 2 );

        bitMask <<= 1;
        bitShift += 2;
    }

    return mortonCode;
}

[numthreads(NUM_THREADS, 1, 1)]
void main(ComputeShaderInput CSIn)
{
    uint4 quantized;

    const uint kBitMortonCode = 10;

    const uint coordinateScale = (1 << kBitMortonCode) - 1;

    if (CSIn.groupIndex == 0)
    {
        gs_AABB = LightsAABB[0];
        gs_AABBRange = 1.0f / (gs_AABB.Max - gs_AABB.Min);
    }

    GroupMemoryBarrierWithGroupSync();

    uint threadIndex = CSIn.dispatchThreadID.x;

    if (threadIndex < LightCountsCB.NumPointLights)
    {
        PointLight point_light = PointLights[threadIndex];
        quantized = (point_light.PositionVS - gs_AABB.Min) * gs_AABBRange * coordinateScale;
        RWPointLightMortonCodes[threadIndex] = MortonCode(quantized.xyz, kBitMortonCode);
        RWPointLightIndicies[threadIndex] = threadIndex;
    }

    if (threadIndex < LightCountsCB.NumSpotLights)
    {
        SpotLight spot_light = SpotLights[threadIndex];
        quantized = (spot_light.PositionVS - gs_AABB.Min) * gs_AABBRange * coordinateScale;
        RWSpotLightMortonCodes[threadIndex] = MortonCode(quantized.xyz, kBitMortonCode);
        RWSpotLightIndicies[threadIndex] = threadIndex;
    }
}