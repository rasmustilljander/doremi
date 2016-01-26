#define NUM_LIGHTS 6

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float3 screenPos : SCREENPOS;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPos : CAMERAPOS;
};

struct LightGridInfo
{
    uint offset;
    uint value;
};

struct Light
{
    int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
    float3 attenuation;
    float intensity;
    float3 color;
    float coneAngle;
    float3 direction;
    float penumAgle;
    float3 position;
    bool enabled;
    float3 pad;
};

struct Plane
{
    float3 N;   // Plane normal.
    float  d;   // Distance to origin.
};
struct Frustum
{
    Plane plane[4];   // left, right, top, bottom frustum planes.
};


StructuredBuffer<uint> o_LightIndexList : register(t1);
StructuredBuffer<uint> t_LightIndexList : register(t2);
StructuredBuffer<LightGridInfo> o_LightGrid : register(t3);
StructuredBuffer<LightGridInfo> t_LightGrid : register(t4);

cbuffer LightInfo : register(b0)
{
    Light light[NUM_LIGHTS];
};

Texture2D ObjTexture : register(t0);

SamplerState ObjSamplerState : register(s0);

float3 CalcDirectionalLight(PixelInputType input, int i)
{
    float3 lightDir;
    float lightIntensity;
    float4 texcolor = float4(0.0, 0.0, 0.0, 1);
    float3 normal = normalize(input.normal);

    lightDir = -light[i].direction;
    lightIntensity = saturate(dot(normal, lightDir));
    return saturate(light[i].color * lightIntensity) * 0.3;
}

float3 CalcSpotLight(PixelInputType input, int i)
{
    return float3(0, 0, 0);
}

float3 CalcPointLight(PixelInputType input, int i)
{
    float4 texcolor = float4(0.5, 0.5, 0.5, 1);
    float3 normal = normalize(input.normal);

    float3 scatteredLight, reflectedLight;
    float attenuation;

    float3 lightDirection = light[i].position - input.worldPos.xyz;
    float lightDistance = length(lightDirection);

    if (lightDistance < 500.0f)
    {
        attenuation = 2.5f /
            (light[i].attenuation[0] + light[i].attenuation[1] * lightDistance +
                light[i].attenuation[2] * lightDistance * lightDistance);

        float3 halfVector = normalize(lightDirection + normalize(-input.worldPos));

        float diffuse = max(0.0, dot(normal, lightDirection));
        float specular = max(0.0, dot(normal, halfVector));

        scatteredLight = light[i].color * diffuse * attenuation;
        reflectedLight = light[i].color * specular * attenuation;
        return min(texcolor.rgb * scatteredLight + reflectedLight, float3(1, 1, 1)) * light[i].intensity;
    }

    return float3(0, 0, 0);
}

float4 PS_main(PixelInputType input) : SV_TARGET
{
    float2 screenPos = input.screenPos.xy / input.screenPos.z;

    float4 texcolor = ObjTexture.Sample(ObjSamplerState, input.texCoord);
    texcolor = saturate(texcolor);
    //return texcolor;
    //texcolor = float4(0.9, 0.9, 0.9, 1);
    
    float3 rgb = float3(0, 0, 0);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (light[i].type == 0)
            rgb += float3(0, 0, 0);
        if (light[i].type == 1)
            rgb += CalcDirectionalLight(input, i);
        if (light[i].type == 2)
            rgb += CalcSpotLight(input, i);
        if (light[i].type == 3)
            rgb += CalcPointLight(input, i);

    }
    Plane test;
    test.N = float3(0, 0, 0);
    
    //return float4(rgb, 1) * texcolor * 3;
    return float4((t_LightGrid[screenPos.x + (screenPos.y * 800)].value) / 6.f, (t_LightGrid[screenPos.x + (screenPos.y * 800)].value) / 6.f, (t_LightGrid[screenPos.x + (screenPos.y * 800)].value) / 6.f, 1);
    //return float4(4.f/6.f, 4.f/6.f, 4.f/6.f, 1);

}