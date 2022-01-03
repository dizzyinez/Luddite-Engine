#include "VTFS/Common.hlsl"

struct VSInput
{
    float3 Pos : ATTRIB0;
    float3 Normal : ATTRIB1;
    float3 Tangent : ATTRIB2;
    float4 Bitangent : ATTRIB3;
    uint4 BoneIndices : ATTRIB4;
    float4 BoneWeights : ATTRIB5;
    float4 Color : ATTRIB6;
    float2 UV : ATTRIB7;
};

struct PSInput 
{ 
    float4 Pos : SV_POSITION;
    float4 PosVS : VIEWSPACEPOS;
};

void main(in  VSInput VSIn,
          out PSInput PSIn) 
{    
    float4 local_pos = float4(VSIn.Pos, 1.0);
    if (BasicModelCameraCB.NumBones > 0)
    {
        float4x4 SkinMat = 
            VSIn.BoneWeights.x * BoneTransforms[VSIn.BoneIndices.x] + 
            VSIn.BoneWeights.y * BoneTransforms[VSIn.BoneIndices.y] + 
            VSIn.BoneWeights.z * BoneTransforms[VSIn.BoneIndices.z] + 
            VSIn.BoneWeights.w * BoneTransforms[VSIn.BoneIndices.w];
        local_pos = mul(SkinMat, local_pos);
    }
    PSIn.Pos = LocalToClip(local_pos);
    PSIn.PosVS = LocalToView(local_pos);
    //PSIn.Pos = mul(BasicModelCameraCB.ModelViewProjection, local_pos);
    //PSIn.PosVS = mul(BasicModelCameraCB.ModelView, float4(VSIn.Pos, 1.0));
}
