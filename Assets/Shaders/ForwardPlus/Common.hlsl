struct Plane
{
    float3 N; //Normal
    float D; //Distance to origin
};

struct Frustum
{
    Plane planes[4]; //left, right, top, bottom
};

//computes a plane from 3 noncollinear counter-clockwise points
Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;
    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;
    plane.N = normalize(cross(v0, v2));
    plane.D = dot(plane.N, p0);
    return plane;
}

cbuffer ScreenToViewParams : register (b3)
{
    float4x4 InverseProjection;
    float2 ViewDimensions;
}

float4 ClipToView(float4 clip)
{
    float4 view = mul(InverseProjection, clip);
    view = view / view.w;
    return view;
}

float4 ScreenToView(float4 screen)
{
    float2 tex_coord = screen.xy / ViewDimensions;
    float4 clip = float4(float2(tex_coord.x, 1.0f - tex_coord.y) * 2.0f - 1.0f, screen.z, screen.w);
    return ClipToView(clip);
}