#include "VTFS/Common.hlsl"

#ifndef NUM_THREADS
#define NUM_THREADS 256
#endif

#define INT_MAX 0xffffffff

groupshared uint gs_Keys[NUM_THREADS];
groupshared uint gs_Values[NUM_THREADS];
groupshared uint gs_E[NUM_THREADS];
groupshared uint gs_F[NUM_THREADS];
groupshared uint gs_D[NUM_THREADS];
groupshared uint gs_TotalFalses;

[numthreads(NUM_THREADS,1,1)]
void main(in ComputeShaderInput CSIn)
{
    const uint NumBits = 30;

    uint b, i;

    gs_Keys[CSIn.groupIndex] = ( CSIn.dispatchThreadID.x < SortParamsCB.NumElements) ? InputKeys[CSIn.dispatchThreadID.x] : INT_MAX;
    gs_Values[CSIn.groupIndex] = ( CSIn.dispatchThreadID.x < SortParamsCB.NumElements) ? InputValues[CSIn.dispatchThreadID.x] : INT_MAX;

    for (b = 0; b < NumBits; ++b)
    {
        gs_E[CSIn.groupIndex] = ( ( gs_Keys[CSIn.groupIndex] >> b) & 1) == 0 ? 1 : 0;

        GroupMemoryBarrierWithGroupSync();

        if (CSIn.groupIndex == 0)
        {
            gs_F[CSIn.groupIndex] = 0;
        }
        else
        {
            gs_F[CSIn.groupIndex] = gs_E[CSIn.groupIndex - 1];
        }

        GroupMemoryBarrierWithGroupSync();

        [unroll]
        for (i = 1; i < NUM_THREADS; i <<= 1)
        {
            uint temp = gs_F[CSIn.groupIndex];

            if (CSIn.groupIndex > i)
            {
                temp += gs_F[CSIn.groupIndex - i];
            }

            GroupMemoryBarrierWithGroupSync();

            gs_F[CSIn.groupIndex] = temp;

            GroupMemoryBarrierWithGroupSync();
        }

        if (CSIn.groupIndex == 0)
        {
            gs_TotalFalses = gs_E[NUM_THREADS - 1] + gs_F[NUM_THREADS - 1];
        }

        GroupMemoryBarrierWithGroupSync();

        gs_D[CSIn.groupIndex] = (gs_E[CSIn.groupIndex] == 1) ? gs_F[CSIn.groupIndex] : CSIn.groupIndex - gs_F[CSIn.groupIndex] + gs_TotalFalses;

        uint key = gs_Keys[CSIn.groupIndex];
        uint value = gs_Values[CSIn.groupIndex];

        GroupMemoryBarrierWithGroupSync();

        gs_Keys[gs_D[CSIn.groupIndex]] = key;
        gs_Values[gs_D[CSIn.groupIndex]] = value;
    }

    OutputKeys[CSIn.dispatchThreadID.x] = gs_Keys[CSIn.groupIndex];
    OutputValues[CSIn.dispatchThreadID.x] = gs_Values[CSIn.groupIndex];
}