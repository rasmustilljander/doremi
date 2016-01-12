#define NUM_LIGHTS 4

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPos : CAMERAPOS;
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
};

cbuffer LightInfo : register(b0)
{
    Light light[NUM_LIGHTS];
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float3 CalcDirectionalLight(PixelInputType input, int i)
{
    return float3(0, 0, 0);
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

    if (lightDistance < 50.0f)
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
    float4 texcolor = ObjTexture.Sample(ObjSamplerState, input.texCoord);
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

    return float4(rgb, 1) + texcolor * 0.15;
}