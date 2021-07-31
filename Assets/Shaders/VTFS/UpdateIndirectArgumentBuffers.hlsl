ByteAddressBuffer ClusterCounter;
RWByteAddressBuffer AssignLightsToClustersIndirectArgumentBuffer;

[numthreads(1,1,1)]
void main()
{
    uint cluster_count = ClusterCounter.Load(0);
    AssignLightsToClustersIndirectArgumentBuffer.Store3(0, uint3(cluster_count, 1, 1));
    // AssignLightsToClustersIndirectArgumentBuffer[0] = cluster_count;
    // AssignLightsToClustersIndirectArgumentBuffer[1] = 1;
    // AssignLightsToClustersIndirectArgumentBuffer[2] = 1;
}