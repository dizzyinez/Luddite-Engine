struct Plane
{
    float3 N; //Normal
    float D; //Distance to origin
}

struct Frustum
{
    Plane plane[4]; //left, right, top, bottom
}

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