cbuffer ShaderConstants
{
    float4x4 g_CameraViewProj;
};

cbuffer ModelConstants
{
    float4x4 g_Transform;
};

// Vertex shader takes two inputs: vertex position and uv coordinates.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
struct VSInput
{
    float3 Pos : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV  : ATTRIB2;
};

struct PSInput 
{ 
    float4 Pos : SV_POSITION; 
    float3 Normal : NORMAL;
    float2 UV  : TEX_COORD; 
};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be indentical.
void main(in  VSInput VSIn,
          out PSInput PSIn) 
{    
    PSIn.Pos = mul(g_CameraViewProj, mul(g_Transform, float4(VSIn.Pos, 1.0)));
    float3 Normal = (VSIn.Normal + 1.0f) * 0.5f;
    PSIn.Normal = float3(mul(g_Transform, float4(Normal, 0.0))).rgb;
    PSIn.UV  = VSIn.UV;
}