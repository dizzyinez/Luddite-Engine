#include "ForwardPlus/Common.hlsl"

#ifndef BLOCK_SIZE
#pragma message("BLOCK_SIZE is undefined. Defaulting to 16")
#define BLOCK_SIZE 16
#endif

cbuffer Constants : register (b4)
{
    uint3 numThreadGroups;
    uint3 numThreads;
}

RWStructuredBuffer<Frustum> g_Frustums : register(u0);

[numThreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(in ComputeShaderInput CSIn)
{
    const float3 eye_pos = float3(0,0,0);

    float4 screen_space[4];
    screen_space[0] = float4( CSIn.dispatchThreadID.xy * BLOCK_SIZE, -1.0f, 1.0f);
    screen_space[1] = float4(float2(CSIn.dispatchThreadID.x + 1, CSIn.dispatchThreadID.y) * BLOCK_SIZE, -1.0f, 1.0f);
    screen_space[1] = float4(float2(CSIn.dispatchThreadID.x, CSIn.dispatchThreadID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f);
    screen_space[1] = float4(float2(CSIn.dispatchThreadID.x + 1, CSIn.dispatchThreadID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f);

    float3 view_space[4];
    view_space[0] = ScreenToView(screen_space[0]).xyz;
    view_space[1] = ScreenToView(screen_space[1]).xyz;
    view_space[2] = ScreenToView(screen_space[2]).xyz;
    view_space[3] = ScreenToView(screen_space[3]).xyz;
    
    Frustum frustum;
    frustum.planes[0] = ComputePlane(eye_pos, view_space[2], view_space[0]);
    frustum.planes[1] = ComputePlane(eye_pos, view_space[1], view_space[3]);
    frustum.planes[2] = ComputePlane(eye_pos, view_space[0], view_space[1]);
    frustum.planes[3] = ComputePlane(eye_pos, view_space[3], view_space[2]);

    if (CSIn.dispatchThreadID.x < numThreads.x && CSIn.dispatchThreadID.y < numThreads.y)
    {
        uint index = CSIn.dispatchThreadID.x + (CSIn.dispatchThreadID.y * numThreads.x);
        g_Frustums[index] = frustum;
    }
}