#include "VTFS/Common.hlsl"
#define NUM_THREADS 1024

cbuffer UpdateLightsCB
{
    float4x4 view_matrix;
};

[numthreads(NUM_THREADS,1,1)]
void main(in ComputeShaderInput CSIn)
{
    uint threadID = CSIn.dispatchThreadID.x;
    //Update Point Light
    if (threadID < LightCountsCB.NumPointLights)
    {
        PointLight point_light = RWPointLights[threadID];
        point_light.PositionVS = mul(view_matrix, float4(point_light.PositionWS.xyz, 1));
        // point_light.PositionVS = mul(float4(point_light.PositionWS.xyz, 1), view_matrix);
        // point_light.PositionVS.z = -point_light.PositionVS.z;
        // point_light.PositionVS = mul(view_matrix, float4(point_light.PositionWS.xy, -point_light.PositionWS.z, 1));
        RWPointLights[threadID] = point_light;
    }
    //Update Spot Light
    if (threadID < LightCountsCB.NumSpotLights)
    {
        SpotLight spot_light = RWSpotLights[threadID];
        spot_light.PositionVS  = mul(view_matrix, float4(spot_light.PositionWS.xyz, 1));
        spot_light.DirectionVS = normalize(mul(view_matrix, float4(spot_light.DirectionWS.xyz, 0)));
        RWSpotLights[threadID] = spot_light;
    }
    //Update Directional Light
    if (threadID < LightCountsCB.NumDirectionalLights)
    {
        DirectionalLight directional_light = RWDirectionalLights[threadID];
        directional_light.DirectionVS = normalize(mul(view_matrix, float4(directional_light.DirectionWS.xyz, 0)));
        RWDirectionalLights[threadID] = directional_light;
    }
}