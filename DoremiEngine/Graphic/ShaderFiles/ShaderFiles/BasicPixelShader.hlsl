#define BLOCK_SIZE 32
#define NUM_LIGHTS 500

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
    float4 glow : SV_Target1;
    float4 depth : SV_Target2;
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

cbuffer ResolutionData : register(b2)
{
    float2 Resolution;
    float2 PaddingResol;
}

Texture2D ObjTexture : register(t0);
Texture2D GlowTexture : register(t5);

SamplerState ObjSamplerState : register(s0);

float3 CalcDirectionalLight(PixelInputType input, Light l, float3 texcolor)
{
    float3 lightDir;
    float lightIntensity;

    lightDir = -l.direction;
    lightIntensity = saturate(dot(input.normal, lightDir));
    return l.color * lightIntensity * texcolor*  l.intensity;
}
/*

*/

float3 CalcSpotLight(PixelInputType input, Light l)
{
    return float3(0, 0, 0);
}

float3 CalcPointLight(PixelInputType input, Light l, float3 texcolor)
{
    float3 lightVec = l.position - input.worldPos.xyz;
    float radius = l.intensity * 20.f;

    float d = length(lightVec);
    if (d > radius)
    {
        return float3(0, 0, 0);
    }

    lightVec /= d;

    float diffuseFactor = dot(lightVec, input.normal);

    if (diffuseFactor < 0.0f)
    {
        return float3(0, 0, 0);
    }

    float att = pow(max(0.0f, 1.0 - (d / radius)), 4.0f);

    float3 toEye = normalize(input.cameraPos - input.worldPos);
    float3 v = reflect(-lightVec, input.normal);


    float specFactor = pow(max(dot(v, toEye), 0.0f), 1.0f) * 0.5;

    return (l.color * att * (diffuseFactor + specFactor)) * texcolor;
}

float3 CalcPointLight2(PixelInputType input, Light l, float3 texcolor)
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
    screenPos.y = (-screenPos.y + 1) * Resolution.y / 2;
    screenPos.x = (screenPos.x + 1) * Resolution.x / 2;

    //calculate which thread group this pixel was in the compute shader stage
    float2 groupID2 = float2((int)(screenPos.x / BLOCK_SIZE), (int)(screenPos.y / BLOCK_SIZE));
    float groupID = groupID2.x + groupID2.y * (Resolution.x / BLOCK_SIZE);
    //extract data from light grid
    int index = o_LightGrid[groupID].offset;
    int value = o_LightGrid[groupID].value;

    float4 texcolor;
    float4 glowcolor;

    if (mapMasks == 0)
    {
        texcolor = float4(color, 1);
        glowcolor = GlowTexture.Sample(ObjSamplerState, float2(0, 0));
    }
    else
    {
        texcolor = ObjTexture.Sample(ObjSamplerState, input.texCoord);
        glowcolor = GlowTexture.Sample(ObjSamplerState, input.texCoord);
    }


    float3 rgb = float3(0, 0, 0);


    Light directionalLight;
    directionalLight.intensity = 1.2f;
    directionalLight.color = float3(0.2f ,0.2f ,0.7f);
    directionalLight.direction = normalize(float3(0.3, -0.8, 0));
    rgb += CalcDirectionalLight(input, directionalLight, texcolor);

    for (int i = index; i < index + value; i++)
        //for (int i = 0; i < 200; i++)
    {
        Light l = light[o_LightIndexList[i]];
        ////Light l = light[i];
        //if (l.type == 0)
        //    rgb += float3(0, 0, 0);
        //else if (l.type == 1)
        //    rgb += CalcDirectionalLight(input, l, texcolor);
        //else if (l.type == 2)
        //    rgb += CalcSpotLight(input, l);
        //else if (l.type == 3)
            rgb += CalcPointLight2(input, l, texcolor.rgb);
    }

    if (glowcolor.r < 0.5)
        output.glow = float4(0, 0, 0, 0);
    else
        output.glow = texcolor * glowcolor.r;

    //if (glowcolor.r < 0.5 &&
    //    glowcolor.g < 0.5 && glowcolor.b < 0.5)
    //    output.glow = float4(0, 0, 0, 0);
    //else
    //    output.glow = saturate(normalize(texcolor) * glowcolor.r) * 2;

    //output.diffuse = float4(rgb, 1) + texcolor * 0.25*;


    output.diffuse = float4(rgb, 1) + texcolor * 0.17f;

    float depth = (input.position.z / input.position.w) * -1 + 1.3;   //Invert and add .3 to the depth as a buffer
    output.depth = float4(depth, depth, depth, 1);

    return output;
}