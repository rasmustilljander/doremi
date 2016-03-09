#define BLOCK_SIZE 32
#define NUM_LIGHTS 500
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080


struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float2 depth : DEPTH;
    float3 screenPos : SCREENPOS;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPos : CAMERAPOS;
};

struct PixelOutputType
{
    float4 diffuse : SV_Target0;
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

cbuffer MaterialData : register(b1)
{
    int mapMasks;
    float3 color;
    float diffuse;
    float3 ambColor;
    float specCosine;
    float3 specColor;
    float specEccentricity;
    float specRollOff;
    float alpha;
    float pad;
};

Texture2D ObjTexture : register(t0);
Texture2D GlowTexture : register(t5);

SamplerState ObjSamplerState : register(s0);

float3 CalcDirectionalLight(PixelInputType input, Light l)
{
    float3 lightDir;
    float lightIntensity;
    float4 texcolor = float4(0.0, 0.0, 0.0, 1);
    float3 normal = normalize(input.normal);

    lightDir = -l.direction;
    lightIntensity = saturate(dot(normal, lightDir));
    return saturate(l.color * lightIntensity) * 0.1;
}

float3 CalcSpotLight(PixelInputType input, Light l)
{
    return float3(0, 0, 0);
}

float3 CalcPointLight(PixelInputType input, Light l, float3 texcolor)
{
    float3 normal = normalize(input.normal);

    float3 scatteredLight, reflectedLight;
    float attenuation;

    float3 lightVec = l.position - input.worldPos.xyz;
    float radius = l.intensity * 25.f;
    float d = length(lightVec);

    if (d < radius)
    {
        float attenuation = pow(max(0.0f, 1.0 - (d / radius)), 20.0f);

        float3 halfVector = normalize(lightVec + normalize(-input.worldPos));

        float diffuse = max(0.0, dot(normal, lightVec));
        float specular = max(0.0, dot(normal, halfVector));

        scatteredLight = l.color * diffuse * attenuation;
        reflectedLight = l.color * specular * attenuation;
        return min(texcolor.rgb * (scatteredLight + reflectedLight), float3(1, 1, 1));
    }

    return float3(0, 0, 0);
}

PixelOutputType PS_main(PixelInputType input)
{
    PixelOutputType output;
    float2 screenPos = input.screenPos.xy / input.screenPos.z;
    screenPos.y = (-screenPos.y + 1) * SCREEN_HEIGHT / 2;
    screenPos.x = (screenPos.x + 1) * SCREEN_WIDTH / 2;

    //calculate which thread group this pixel was in the compute shader stage
    float2 groupID2 = float2((int)screenPos.x / BLOCK_SIZE, (int)screenPos.y / BLOCK_SIZE);
    float groupID = groupID2.x + (groupID2.y * ceil(SCREEN_WIDTH / BLOCK_SIZE));
    //extract data from light grid
    int index = t_LightGrid[groupID].offset;
    int value = t_LightGrid[groupID].value;

    float4 texcolor = ObjTexture.Sample(ObjSamplerState, input.texCoord);
    float4 glowcolor = GlowTexture.Sample(ObjSamplerState, input.texCoord);

    texcolor.w *= alpha;

    float3 rgb = float3(0, 0, 0);

    for (int i = index; i < index + value; i++)
    {
        Light l = light[t_LightIndexList[i]];
        rgb += CalcPointLight(input, l, texcolor);
    }

    if (mapMasks == 0)
    {
        output.diffuse = float4(color, 1);
    }
    else
    {
        output.diffuse = float4(rgb * 2, 0) + texcolor;
    }

    return output;


}