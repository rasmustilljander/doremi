struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};


Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(PixelInputType input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.texCoord);
}