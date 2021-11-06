#include "VTFS/Common.hlsl"

const float PI = 3.14159265359;

TextureCube<float4> g_IrradianceMap;
SamplerState        g_IrradianceMap_sampler;

TextureCube<float4> g_RadianceMap;
SamplerState        g_RadianceMap_sampler;

TextureCube<float4> g_Skybox;
SamplerState        g_Skybox_sampler;

struct SurfaceProps
{
    float3 PosWS;
    float Roughness;
    float3 Diffuse;
    float Metallic;
    float3 ViewDir;
    float dotNV;
    float3 Normal;
    float3 F0;
};

float3 FresnelSchlick(float cos, float3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cos, 0.0), 5.0);
}

float3 FresnelSchlickRoughness(float cos, float3 F0, float roughness)
{
    return F0 + max(float3(1.0 - roughness) - F0, 0.0) * pow(max(1.0 - cos, 0.0), 5.0);
}

float DistributionGGX(float dotNH, float roughness)
{
    float alpha = max(0.001f, roughness * roughness);
    float alpha2 = alpha * alpha;
    float dotNH2 = dotNH*dotNH;

    float denom = ((dotNH2 * (alpha2 - 1.0)) + 1.0);
    denom = PI * denom * denom;

    return alpha2 / denom;
}

float GeometrySchlickGGX(float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    return (dotNV / ((dotNV * (1.0 - k)) + k));
}

float GeometrySmith(float dotNV, float dotNL, float roughness)
{
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    return ggx1 * ggx2;
}

float3 DoPointLight(SurfaceProps props, PointLight point_light)
{
    float3 light_dir = normalize(point_light.PositionWS - props.PosWS);

    //attenuation
    float distance = distance(point_light.PositionWS, props.PosWS);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = point_light.Color * attenuation * point_light.Intensity;

    //calculate vectors & dot products
    float3 H = normalize(props.ViewDir + light_dir);
    float dotHV = saturate(dot(H, props.ViewDir));
    float dotNL = saturate(dot(props.Normal, light_dir));
    float dotNH = saturate(dot(props.Normal, H));

    //cook-rottancebrdf
    float NDF = DistributionGGX(dotNH, props.Roughness);
    float G = GeometrySmith(props.dotNV, dotNL, props.Roughness);
    float3 F = FresnelSchlick(dotHV, props.F0);
    //float3 F = FresnelSchlickRoughness(dotHV, props.F0, props.Roughness);

    float3 kS = F;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - props.Metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * props.dotNV * dotNL;
    float3 specular = numerator / max(denominator, 0.001);
    return (kD * props.Diffuse / PI + specular) * radiance * dotNL;
}

float3 DoSpotLight(SurfaceProps props, SpotLight spot_light)
{
    float3 light_dir = normalize(spot_light.PositionWS - props.PosWS);
    float minCos = cos(spot_light.HalfAngle);
    float maxCos = lerp(minCos, 1, 0.5f);
    float cosAngle = dot(spot_light.DirectionWS.xyz, -light_dir);
    float spotIntensity = smoothstep(minCos, maxCos, cosAngle);

    //attenuation
    float distance = distance(spot_light.PositionWS, props.PosWS);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = spot_light.Color * attenuation * spot_light.Intensity;

    //calculate vectors & dot products
    float3 H = normalize(props.ViewDir + light_dir);
    float dotHV = saturate(dot(H, props.ViewDir));
    float dotNL = saturate(dot(props.Normal, light_dir));
    float dotNH = saturate(dot(props.Normal, H));

    //cook-rottancebrdf
    float NDF = DistributionGGX(dotNH, props.Roughness);
    float G = GeometrySmith(props.dotNV, dotNL, props.Roughness);
    // float3 F = FresnelSchlick(dotHV, props.F0);
    float3 F = FresnelSchlickRoughness(dotHV, props.F0, props.Roughness);

    float3 kS = F;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - props.Metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * props.dotNV * dotNL;
    float3 specular = numerator / max(denominator, 0.001);
    return (kD * props.Diffuse / PI + specular) * radiance * dotNL * spotIntensity;
}

float3 DoDirectionalLight(SurfaceProps props, DirectionalLight directional_light)
{
    float3 light_dir = -normalize(directional_light.DirectionWS);

    //attenuation
    float3 radiance = directional_light.Color * directional_light.Intensity;

    //calculate vectors & dot products
    float3 H = normalize(props.ViewDir + light_dir);
    float dotHV = saturate(dot(H, props.ViewDir));
    float dotNL = saturate(dot(props.Normal, light_dir));
    float dotNH = saturate(dot(props.Normal, H));

    //cook-rottancebrdf
    float NDF = DistributionGGX(dotNH, props.Roughness);
    float G = GeometrySmith(props.dotNV, dotNL, props.Roughness);
    // float3 F = FresnelSchlick(dotHV, props.F0);
    float3 F = FresnelSchlickRoughness(dotHV, props.F0, props.Roughness);

    float3 kS = F;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - props.Metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * props.dotNV * dotNL;
    float3 specular = numerator / max(denominator, 0.001);
    return (kD * props.Diffuse / PI + specular) * radiance * dotNL;
}

float3 DoIBL(SurfaceProps props)
{
    float3 F = FresnelSchlickRoughness(props.dotNV, props.F0, props.Roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - props.Metallic;
    float3 irradiance = g_IrradianceMap.Sample(g_IrradianceMap_sampler, props.Normal).rgb;
    float3 diffuse = irradiance * props.Diffuse;//albedo;

    float3 R = reflect(-props.ViewDir, props.Normal);

    const float MAX_REFLECTION_LOD = 9.0;
    float3 radiance = g_RadianceMap.SampleLevel(g_RadianceMap_sampler, R, MAX_REFLECTION_LOD * props.Roughness);
    float3 specular = radiance * F;
    float3 ambient = kD * diffuse + specular;
    return ambient;
    //return g_IrradianceMap.Sample(g_IrradianceMap_sampler, props.ViewDir).rgb;
    //return props.ViewDir.rgb;
}

float3 DoPBR(SurfaceProps props, float2 clip_xy, float view_z)
{
    uint i;
    uint start_offset, light_count;
    uint light_index;

    uint3 cluster_index_3D = ComputeClusterIndex3D(clip_xy, view_z);
    uint cluster_index_1D = ComputeClusterIndex1D(cluster_index_3D);
    float3 Lo = float3(0.05);
    //Point Lights
    {
        start_offset = PointLightGrid[cluster_index_1D].x;
        light_count = PointLightGrid[cluster_index_1D].y;
        

        for (i = 0; i < light_count; ++i)
        {
            light_index = PointLightIndexList[start_offset+i];
            // if (!PointLights[light_index].Enabled) continue;
            Lo += DoPointLight(props, PointLights[light_index]);
        }       
    }
    //Spot Lights
    {
        start_offset = SpotLightGrid[cluster_index_1D].x;
        light_count = SpotLightGrid[cluster_index_1D].y;
        for (i = 0; i < light_count; ++i)
        {
            light_index = SpotLightIndexList[start_offset+i];
            Lo += DoSpotLight(props, SpotLights[light_index]);
        }
    }
    //Directional Lights
    {
        for (i = 0; i < LightCountsCB.NumDirectionalLights; i++)
        {
            Lo += DoDirectionalLight(props, DirectionalLights[i]);
        }
    }
    //IBL
    float3 color = Lo + DoIBL(props);
    //color = color / (color + float3(1.0));
    //color = pow(color, float3(1.0/2.2));
    //return float4(color, 1.0);
    return color;
}

SurfaceProps CreateSurfaceProps(float4 PosWS, float3 Normal, float3 Diffuse, float Roughness, float Metallic)
{
    SurfaceProps props;
    props.PosWS = PosWS;
    float3 cam_pos = ClipToWorld(float4(0,0,-1,1));
    float3 view_dir = normalize(cam_pos - PosWS);
    props.ViewDir = view_dir;
    props.dotNV = saturate(dot(Normal, view_dir));
    props.Normal = Normal;
    props.F0 = lerp(float3(0.04), Diffuse, float3(Metallic));
    props.Roughness = Roughness;
    props.Diffuse = Diffuse;
    props.Metallic = Metallic;
    return props;
}
