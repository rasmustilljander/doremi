cbuffer MatrixBuffer : register(b1)
{
	matrix worldMatrix;
};

struct VertexInputType
{
	float2 position : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

VOut VS_main( VertexInputType input)
{
	VOut output;

	output.texCoord = input.texCoord;
    input.position = mul(float4(input.position, 0.0f, 1.0f), worldMatrix);
	// Output position of the vertex, in clip space
	// map [0..800][0..600] to [-1..1][-1..1]
	float2 vertexPosition_homoneneousspace = input.position - float2(400.0f, 400.0f); // [0..800][0..600] -> [-400..400][-300..300]
	vertexPosition_homoneneousspace /= float2(400.0f, 400.0f);

	output.position = float4(vertexPosition_homoneneousspace, 0.0f, 1.0f);
	return output;
}