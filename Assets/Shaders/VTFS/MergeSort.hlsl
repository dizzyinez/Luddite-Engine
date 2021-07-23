#include "VTFS/Common.hlsl"

#ifndef NUM_THREADS
#define NUM_THREADS 256
#endif

#ifndef NUM_VALUES_PER_THREAD
#define NUM_VALUES_PER_THREAD 8
#endif

#define NUM_VALUES_PER_THREAD_GROUP (NUM_THREADS * NUM_VALUES_PER_THREAD)

#define INT_MAX 0xffffffff

groupshared uint gs_Keys[NUM_VALUES_PER_THREAD_GROUP];
groupshared uint gs_Values[NUM_VALUES_PER_THREAD_GROUP];

int MergePath(int a0, int aCount, int b0, int bCount, int diag, bool bUseSharedMem)
{
    int begin = max(0, diag - bCount);
    int end = min(diag, aCount);

    while (begin < end)
    {
        int mid = (begin + end) >> 1;
        uint a = bUseSharedMem ? gs_Keys[a0 + mid] : InputKeys[a0 + mid];
        uint b = bUseSharedMem ? gs_Keys[b0 + diag - 1 - mid] : InputKeys[b0 + diag - 1 - mid];
        if (a<b)
        {
            begin = mid + 1;
        }
        else
        {
            end = mid;
        }
    }
    return begin;
}

[numthreads(NUM_THREADS,1,1)]
void merge_path_partitions(ComputeShaderInput CSIn)
{
    uint chunk_size = SortParamsCB.ChunkSize;
    uint num_values_per_sort_group = min(chunk_size * 2, SortParamsCB.NumElements);
    uint num_chunks = ceil((float)SortParamsCB.NumElements / (float)chunk_size);
    uint num_sort_groups = num_chunks / 2;
    uint num_partitions_per_sort_group = ceil((float)num_values_per_sort_group / (float)NUM_VALUES_PER_THREAD_GROUP) + 1;
    uint sort_group = CSIn.dispatchThreadID.x / num_partitions_per_sort_group;
    uint partition_in_sort_group = CSIn.dispatchThreadID.x % num_partitions_per_sort_group;
    uint global_partition = (sort_group * num_partitions_per_sort_group) + partition_in_sort_group;
    uint max_partitions = num_sort_groups * num_partitions_per_sort_group;

    if (global_partition < max_partitions)
    {
        int a0 = sort_group * num_values_per_sort_group;
        int a1 = min(a0 + chunk_size, SortParamsCB.NumElements);
        int aCount = a1 - a0;
        int b0 = a1;
        int b1 = min(b0 + chunk_size, SortParamsCB.NumElements);
        int bCount = b1 - b0;
        int num_values = aCount + bCount;
        int diag = min(partition_in_sort_group * NUM_VALUES_PER_THREAD_GROUP, num_values);
        int merge_path = MergePath(a0, aCount, b0, bCount, diag, false);
        RWMergePathPartitions[global_partition] = merge_path;
    }
}

void SerialMerge( int a0, int a1, int b0, int b1, int diag, uint numValues, uint out0 )
{
    uint i, aKey, bKey, aValue, bValue;

    aKey = gs_Keys[a0];
    bKey = gs_Keys[b0];

    aValue = gs_Values[a0];
    bValue = gs_Values[b0];

    [unroll]
    for ( i = 0; i < NUM_VALUES_PER_THREAD && diag + i < numValues; ++i )
    {
        if ( b0 >= b1 || ( a0 < a1 && aKey < bKey ) )
        {
            OutputKeys[out0 + diag + i] = aKey;
            OutputValues[out0 + diag + i] = aValue;

            ++a0;

            aKey = gs_Keys[a0];
            aValue = gs_Values[a0];
        }
        else
        {
            OutputKeys[out0 + diag + i] = bKey;
            OutputValues[out0 + diag + i] = bValue;

            ++b0;

            bKey = gs_Keys[b0];
            bValue = gs_Values[b0];
        }
    }
}

[numthreads(NUM_THREADS,1,1)]
void merge_sort(ComputeShaderInput CSIn)
{
    uint i, key, value;
    uint chunk_size = SortParamsCB.ChunkSize;
    uint num_chunks = ceil((float)SortParamsCB.NumElements / (float)chunk_size);
    uint num_sort_groups = max(num_chunks / 2, 1);
    uint num_values_per_sort_group = min(chunk_size * 2, SortParamsCB.NumElements);
    uint num_thread_groups_per_sort_group = ceil((float)num_values_per_sort_group / (float)NUM_VALUES_PER_THREAD_GROUP);
    uint num_partitions_per_sort_group = num_thread_groups_per_sort_group + 1;
    uint sort_group = CSIn.groupID.x / num_partitions_per_sort_group;
    uint partition = CSIn.groupID.x % num_partitions_per_sort_group;
    uint global_partition = (sort_group * num_partitions_per_sort_group) + partition;

    int merge_path0 = MergePathPartitions[global_partition];
    int merge_path1 = MergePathPartitions[global_partition + 1];
    int diag0 = min( partition * NUM_VALUES_PER_THREAD_GROUP, num_values_per_sort_group );
    int diag1 = min( (partition + 1) * NUM_VALUES_PER_THREAD_GROUP, num_values_per_sort_group );

    int chunk_offset_A0 = min(sort_group * num_values_per_sort_group, SortParamsCB.NumElements);
    int chunk_offset_A1 = min(chunk_offset_A0 + chunk_size, SortParamsCB.NumElements);
    int chunk_size_A = chunk_offset_A1 - chunk_offset_A0;

    int chunk_offset_B0 = chunk_offset_A1;
    int chunk_offset_B1 = min(chunk_offset_B0 + chunk_size, SortParamsCB.NumElements);
    int chunk_size_B = chunk_offset_B1 - chunk_offset_B0;

    uint num_values = chunk_size_A + chunk_size_B;

    int a0 = merge_path0;
    int a1 = merge_path1;
    int numA = min(a1 - a0, chunk_size_A);

    int b0 = diag0 - merge_path0;
    int b1 = diag1 - merge_path1;
    int numB = min(b1 - b0, chunk_size_B);

    int diag = CSIn.groupIndex * NUM_VALUES_PER_THREAD;

    [unroll]
    for (i = 0; i < NUM_VALUES_PER_THREAD; ++i)
    {
        int a = a0 + diag + i;
        int b = b0 + (a - a1);
        if (a < a1)
        {
            key = InputKeys[chunk_offset_A0 + a];
            value = InputValues[chunk_offset_A0 + a];
        }
        else
        {
            key = InputKeys[chunk_offset_B0 + b];
            value = InputValues[chunk_offset_B0 + b];
        }
        gs_Keys[diag + i] = key;
        gs_Values[diag + i] = value;
    }

    GroupMemoryBarrierWithGroupSync();

    int merge_path = MergePath(0, numA, numA, numB, diag, true);

    SerialMerge(merge_path, numA, numA + diag - merge_path, numA + numB, diag0 + diag, num_values, chunk_offset_A0);
}