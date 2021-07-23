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
    float4 PosWS : WORLDSPACEPOS;
    float3 Normal : NORMAL;
    float2 UV  : TEX_COORD; 
};

void main(in  VSInput VSIn,
          out PSInput PSIn) 
{    
    PSIn.Pos = mul(BasicModelCameraCB.ModelViewProjection, float4(VSIn.Pos, 1.0));
    PSIn.PosVS = mul(BasicModelCameraCB.ModelView, float4(VSIn.Pos, 1.0));
    PSIn.PosWS = mul(BasicModelCameraCB.Model, float4(VSIn.Pos, 1.0));

    // float3 Normal = (VSIn.Normal + 1.0f) * 0.5f;
    float3 Normal = VSIn.Normal;
    PSIn.Normal = float3(mul(BasicModelCameraCB.Model, float4(Normal, 0.0)));
    PSIn.UV  = VSIn.UV;
}