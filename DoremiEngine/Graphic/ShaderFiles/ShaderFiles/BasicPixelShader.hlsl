struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};


//Texture2D ObjTexture : register(t0);
//SamplerState ObjSamplerState : register(s0);

float4 PS_main(PixelInputType input) : SV_TARGET
{
    return float4(1,0,0,1);
}