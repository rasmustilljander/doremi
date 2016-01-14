struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD0;

};


//Texture2D ObjTexture : register(t0);
TextureCube SkyMap : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 PS_main(PixelInputType input) : SV_TARGET
{
	//Text shader
	float4 color;
	color = SkyMap.Sample(ObjSamplerState, input.texCoord);
	return color;
}