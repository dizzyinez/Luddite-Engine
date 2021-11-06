float4 ClipToView(float4 clip)
{
    float4 view = mul(CameraCB.InverseProjection, clip);
    view = view / view.w;
    return view;
}

float4 ViewToWorld(float4 view)
{
    return mul(CameraCB.InverseView, view);
}

float4 ClipToWorld(float4 clip)
{
    float4 view = mul(CameraCB.InverseProjection, clip);
    view = view / view.w;
    return mul(CameraCB.InverseView, view);
}

float4 ScreenToView(float4 screen)
{
    float2 tex_coord = screen.xy / CameraCB.ViewDimensions;
    float4 clip = float4(float2(tex_coord.x, 1.0f - tex_coord.y) * 2.0f - 1.0f, DepthToNormalizedDeviceZ(screen.z), screen.w);
    return ClipToView(clip);
}

float4 LocalToClip(float4 local)
{
    return mul(BasicModelCameraCB.ModelViewProjection, local);
}

float4 LocalToView(float4 local)
{
    return mul(BasicModelCameraCB.ModelView, local);
}

float4 LocalToWorld(float4 local)
{
    return mul(BasicModelCameraCB.Model, local);
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

uint ComputeClusterIndex1D(uint3 cluster_index_3D)
{
    return cluster_index_3D.x + ( ClusterCB.grid_dim.x * ( cluster_index_3D.y + ClusterCB.grid_dim.y * cluster_index_3D.z ) );
}

uint3 ComputeClusterIndex3D( uint cluster_index_1D )
{
    return uint3(
    cluster_index_1D % ClusterCB.grid_dim.x,
    cluster_index_1D % (ClusterCB.grid_dim.x * ClusterCB.grid_dim.y) / ClusterCB.grid_dim.x,
    cluster_index_1D / (ClusterCB.grid_dim.x * ClusterCB.grid_dim.y)
    );
}

uint3 ComputeClusterIndex3D( float2 screen_pos, float view_z)
{
    uint i = screen_pos.x / ClusterCB.size.x;
    uint j = screen_pos.y / ClusterCB.size.y;

    uint k = log(-view_z / ClusterCB.view_near) * ClusterCB.log_grid_dim_y;
    return uint3(i, j, k);
}

bool IntersectLinePlane(float3 a, float3 b, Plane p, out float3 q)
{
    float3 ab = b - a;
    float t = (p.D - dot(p.N, a)) / dot(p.N, ab);
    bool intersect = (t >= 0.0f && t <= 1.0f);
    q = float3(0,0,0);
    if (intersect)
    {
        q = a + t * ab;
    }
    return intersect;

}
// Compute the square distance between a point p and an AABB b.
// Source: Real-time collision detection, Christer Ericson (2005)
float SqDistancePointAABB( float3 p, AABB b )
{
    float sqDistance = 0.0f;

    for ( int i = 0; i < 3; ++i )
    {
        float v = p[i];

        if ( v < b.Min[i] ) sqDistance += pow( b.Min[i] - v, 2 );
        if ( v > b.Max[i] ) sqDistance += pow( v - b.Max[i], 2 );
    }

    return sqDistance;
}

// Check to see if a sphere is interesecting an AABB
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsideAABB( Sphere sphere, AABB aabb )
{
    float sqDistance = SqDistancePointAABB( sphere.c, aabb );

    return sqDistance <= sphere.r * sphere.r;
}

// Check to see if on AABB intersects another AABB.
// Source: Real-time collision detection, Christer Ericson (2005)
bool AABBIntersectAABB( AABB a, AABB b )
{
    bool result = true;

    [unroll]
    for ( int i = 0; i < 3; ++i )
    {
        result = result && ( a.Max[i] >= b.Min[i] && a.Min[i] <= b.Max[i] );
    }

    return result;
}
