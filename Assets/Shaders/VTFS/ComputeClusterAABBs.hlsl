#include "VTFS/Common.hlsl"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

[numthreads(BLOCK_SIZE, 1, 1)]
void main(in ComputeShaderInput CSIn)
{
    uint cluster_index_1D = CSIn.dispatchThreadID.x;
    uint3 cluster_index_3D = ComputeClusterIndex3D(cluster_index_1D);

    Plane near_plane = {float3(0.0f, 0.0f, 1.0f), -ClusterCB.view_near * pow(abs(ClusterCB.near_k), cluster_index_3D.z)};
    Plane far_plane =  {float3(0.0f, 0.0f, 1.0f), -ClusterCB.view_near * pow(abs(ClusterCB.near_k), cluster_index_3D.z + 1)};

    float4 p_min = float4(cluster_index_3D.xy * ClusterCB.size.xy, 1.0f, 1.0f);
    float4 p_max = float4((cluster_index_3D.xy + 1) * ClusterCB.size.xy, 1.0f, 1.0f);
    p_min = ScreenToView(p_min);
    p_max = ScreenToView(p_max);

    float3 near_min, near_max, far_min, far_max;

    float3 eye = float3(0,0,0);
    IntersectLinePlane(eye, (float3)p_min, near_plane, near_min);
    IntersectLinePlane(eye, (float3)p_max, near_plane, near_max);
    IntersectLinePlane(eye, (float3)p_min, far_plane, far_min);
    IntersectLinePlane(eye, (float3)p_max, far_plane, far_max);

    float3 aabb_min = min(near_min, min(near_max, min(far_min, far_max)));
    float3 aabb_max = max(near_min, max(near_max, max(far_min, far_max)));
    AABB aabb = {float4(aabb_min, 1.0f), float4(aabb_max, 1.0f)};
    RWClusterAABBs[cluster_index_1D] = aabb;
}