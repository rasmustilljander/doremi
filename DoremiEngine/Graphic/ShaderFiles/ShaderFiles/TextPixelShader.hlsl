struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;

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
SamplerState ObjSamplerState : register(s0);

float4 PS_main(PixelInputType input) : SV_TARGET
{
	//Text shader
	float4 color;
	color = ObjTexture.Sample(ObjSamplerState, input.texCoord);
    color.w *= alpha;
	return color;
	
	

}