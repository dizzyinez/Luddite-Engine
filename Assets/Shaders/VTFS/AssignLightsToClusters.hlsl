#include "VTFS/Common.hlsl"

#define NUM_THREADS 32
#define MAX_LIGHTS 2048

// The number of child nodes (excluding leaves)
// given the number of levels of the BVH.
// for( i = l-1 ... 0 ) { N += 32 ^ i }
static const uint NumChildNodes[] = {
    1,          // 1 level   =32^0
    33,         // 2 levels  +32^1
    1057,       // 3 levels  +32^2
    33825,      // 4 levels  +32^3
    1082401,    // 5 levels  +32^4
    34636833,   // 6 levels  +32^5
};

// Using a stack of node IDs to traverse the BVH was inspired by:
// Source: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
// Author: Tero Karras (NVIDIA)
// Retrieved: September 13, 2016
groupshared uint gs_NodeStack[1024];    // This should be enough to push 32 layers of nodes (32 nodes per layer).
groupshared uint gs_StackPtr;           // The current index in the node stack.
groupshared uint gs_ParentIndex;        // The index of the parent node in the BVH that is currently being processed.

groupshared uint gs_ClusterIndex1D;
groupshared AABB gs_ClusterAABB;

groupshared uint gs_PointLightCount;
groupshared uint gs_SpotLightCount;
groupshared uint gs_PointLightStartOffset;
groupshared uint gs_SpotLightStartOffset;
groupshared uint gs_PointLightList[MAX_LIGHTS];
groupshared uint gs_SpotLightList[MAX_LIGHTS];

#define AppendLight( lightIndex, counter, lightList ) \
    InterlockedAdd( counter, 1, index ); \
    if ( index < MAX_LIGHTS ) \
    { \
        lightList[index] = lightIndex; \
    }

void PushNode( uint nodeIndex )
{
    int stackPtr;    
    InterlockedAdd( gs_StackPtr, 1, stackPtr );
    
    if ( stackPtr < 1024 )
    {
        gs_NodeStack[stackPtr] = nodeIndex;
    }
}

uint PopNode()
{
    uint nodeIndex = 0;
    int stackPtr;
    InterlockedAdd( gs_StackPtr, -1, stackPtr );
    
    if ( stackPtr > 0 && stackPtr < 1024 )
    {
        nodeIndex = gs_NodeStack[stackPtr-1];
    }

    return nodeIndex;
}

// Get the index of the the first child node in the BVH.
uint GetFirstChild( uint parentIndex, uint numLevels )
{
    return ( numLevels > 0 ) ? parentIndex * 32 + 1 : 0;
}

// Check to see if an index of the BVH is a leaf.
bool IsLeafNode( uint childIndex, uint numLevels )
{
    return ( numLevels > 0 ) ? childIndex > ( NumChildNodes[numLevels - 1] - 1 ): true;
}

// Get the index of a leaf node given the node ID in the BVH.
uint GetLeafIndex( uint nodeIndex, uint numLevels )
{
    return ( numLevels > 0 ) ? nodeIndex - NumChildNodes[numLevels - 1] : nodeIndex;
}

[numthreads(NUM_THREADS,1,1)]
void main(ComputeShaderInput CSIn)
{
    uint i, index;
    uint child_offset = CSIn.groupIndex;

    if (CSIn.groupIndex == 0)
    {
        gs_PointLightCount = 0;
        gs_SpotLightCount = 0;
        gs_StackPtr = 0;
        gs_ParentIndex = 0;

        gs_ClusterIndex1D = UniqueClusters[CSIn.groupID.x];
        gs_ClusterAABB = ClusterAABBs[gs_ClusterIndex1D];

        // Push the root node (at index 0) on the node stack.
        PushNode( 0 );
    }

    GroupMemoryBarrierWithGroupSync();

    do
    {
        uint child_index = GetFirstChild(gs_ParentIndex, BVHParamsCB.PointLightLevels) + child_offset;

        if (IsLeafNode(child_index, BVHParamsCB.PointLightLevels))
        {
            uint leaf_index = GetLeafIndex(child_index, BVHParamsCB.PointLightLevels);
            if (leaf_index < LightCountsCB.NumPointLights)
            {
                uint light_index = PointLightIndicies[leaf_index];
                PointLight point_light = PointLights[light_index];
                Sphere sphere = {point_light.PositionVS.xyz, point_light.Range};
                if (point_light.Enabled && SphereInsideAABB(sphere, gs_ClusterAABB))
                {
                    AppendLight(light_index, gs_PointLightCount, gs_PointLightList);
                }
            }
        }
        else if (AABBIntersectAABB(gs_ClusterAABB, PointLightBVH[child_index]))
        {
            PushNode(child_index);
        }
        GroupMemoryBarrierWithGroupSync();

        if (CSIn.groupIndex == 0)
        {
            gs_ParentIndex = PopNode();
        }

        GroupMemoryBarrierWithGroupSync();
    } while (gs_ParentIndex > 0);

    GroupMemoryBarrierWithGroupSync();

    if (CSIn.groupIndex == 0)
    {
        gs_StackPtr == 0;
        gs_ParentIndex = 0;
        PushNode(0);
    }

    GroupMemoryBarrierWithGroupSync();

    do
    {
        uint child_index = GetFirstChild(gs_ParentIndex, BVHParamsCB.SpotLightLevels) + child_offset;

        if (IsLeafNode(child_index, BVHParamsCB.SpotLightLevels))
        {
            uint leaf_index = GetLeafIndex(child_index, BVHParamsCB.SpotLightLevels);
            if (leaf_index < LightCountsCB.NumSpotLights)
            {
                uint light_index = SpotLightIndicies[leaf_index];
                SpotLight spot_light = SpotLights[light_index];
                Sphere sphere = {spot_light.PositionVS.xyz, spot_light.Range};

                if (spot_light.Enabled && SphereInsideAABB(sphere, gs_ClusterAABB))
                {
                    AppendLight(light_index, gs_SpotLightCount, gs_SpotLightList);
                }
            }
        }
        else if (AABBIntersectAABB(gs_ClusterAABB, SpotLightBVH[child_index]))
        {
            PushNode(child_index);
        }

        GroupMemoryBarrierWithGroupSync();

        if (CSIn.groupIndex == 0)
        {
            gs_ParentIndex = PopNode();
        }

        GroupMemoryBarrierWithGroupSync();
    } while(gs_ParentIndex > 0);

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