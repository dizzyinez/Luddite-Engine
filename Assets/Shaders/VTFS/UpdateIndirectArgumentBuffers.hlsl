// cbuffer UpdateClustersCB
// {
//     bool UpdateUniqueClusters;
// }

ByteAddressBuffer ClusterCounter;
RWByteAddressBuffer AssignLightsToClustersIndirectArgumentBuffer;
// RWByteAddressBuffer DebugClustersIndirectArgumentBuffer

[numthreads(1,1,1)]
void main()
{
    uint cluster_count = ClusterCounter.Load(0);
    AssignLightsToClustersIndirectArgumentBuffer.Store3(0, uint3(cluster_count, 1, 1));
}