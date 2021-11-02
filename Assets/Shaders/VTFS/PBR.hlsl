#include "VTFS/Common.hlsl"

const float PI = 3.14159265359;

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

// float3 DoDirectionalLight()
// {

// }
