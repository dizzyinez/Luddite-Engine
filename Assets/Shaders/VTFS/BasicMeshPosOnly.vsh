#include "VTFS/Common.hlsl"

struct VSInput
{
    float3 Pos : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV  : ATTRIB2;
};

struct PSInput 
{ 
    float4 Pos : SV_POSITION;
    float4 PosVS : VIEWSPACEPOS;
};

void main(in  VSInput VSIn,
          out PSInput PSIn) 
{    
    PSIn.Pos = mul(BasicModelCameraCB.ModelViewProjection, float4(VSIn.Pos, 1.0));
    PSIn.PosVS = mul(BasicModelCameraCB.ModelView, float4(VSIn.Pos, 1.0));
    // PSIn.PosVS.z = 1000.0;
}