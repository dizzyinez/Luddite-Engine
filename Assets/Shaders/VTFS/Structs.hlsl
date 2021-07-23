struct Plane
{
    float3 N; //Normal
    float D; //Distance to origin
};

struct Sphere
{
    float3 c;
    float r;
};

struct Cone
{
    float3 T;
    float h;
    float3 d;
    float r;
};

struct AABB
{
    float4 Min;
    float4 Max;
};

struct Frustum
{
    Plane planes[4]; //left, right, top, bottom
};

struct PointLight
{
    float4 PositionWS;
    float4 PositionVS;
    float3 Color;
    float Range;
    float Intensity;
    uint Enabled;
    uint Selected;
    float Padding;
};

struct SpotLight
{
    float4 PositionWS;
    float4 PositionVS;
    float4 DirectionWS;
    float4 DirectionVS;
    float3 Color;
    float HalfAngle;
    float Range;
    float Intensity;
    uint Enabled;
    uint Selected;
};

struct DirectionalLight
{
    float4 DirectionWS;
    float4 DirectionVS;
    float3 Color;
    float Intensity;
    uint Enabled;
    uint Selected;
    uint2 Padding;
};