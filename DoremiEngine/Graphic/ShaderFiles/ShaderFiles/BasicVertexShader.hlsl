cbuffer CameraMatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPosition;
    float pad;
};

cbuffer ModelMatrixBuffer : register(b1)
{
    matrix worldMatrix;
};

struct VertexInputType
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float2 texCoord : TEXCOORD;
    float3 normal: NORMAL;
    float3 cameraPos : CAMERAPOS;
};

VOut VS_main(VertexInputType input, uint instanceID : SV_InstanceID)
{
    VOut output;
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);



    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    output.normal = mul(float4(input.normal, 0.0f), worldMatrix);
    output.texCoord = input.texCoord;
    output.cameraPos = cameraPosition;
    return output;
}