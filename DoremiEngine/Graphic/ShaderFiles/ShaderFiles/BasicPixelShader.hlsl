#define NUM_LIGHTS 3

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
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

cbuffer LightInfo
{
    Light light[NUM_LIGHTS];
    float3 cameraPos;
    float pad;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 PS_main(PixelInputType input) : SV_TARGET
{
    //return ObjTexture.Sample(ObjSamplerState, input.texCoord);
    return float4(light[0].color, 1);
}