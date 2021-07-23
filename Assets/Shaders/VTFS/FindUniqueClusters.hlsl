#include "VTFS/Common.hlsl"
#define NUM_THREADS 1024

[numthreads(NUM_THREADS, 1, 1)]
void main(in ComputeShaderInput CSIn)
{
    uint clusterID = CSIn.dispatchThreadID.x;
    if (ClusterFlags[clusterID])
    {
        uint i = RWUniqueClusters.IncrementCounter();
        RWUniqueClusters[i] = clusterID;
    }
}