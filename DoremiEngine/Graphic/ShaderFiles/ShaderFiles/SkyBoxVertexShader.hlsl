cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
};

cbuffer WVP : register(b1)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix inverseProjection;
    float3 cameraPosition;
    float pad;
}

struct VertexInputType
{
	float3 position : POSITION;
};

struct VOut
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD0;
};

VOut VS_main( VertexInputType input)
{
	VOut output;
    // ww för att z ska vara så långt bak det går
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix).xyww;
    output.texCoord = input.position;

	return output;
}