cbuffer SkeletalAnimationTransformBuffer : register(b2)
{
    // We can have 96 bones at max TODOLH reduce after TA decides how many bones they want
    matrix boneTransforms[40];
}

cbuffer CameraMatrixBuffer : register(b1)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix inverseProjection;
    float3 cameraPosition;
    float pad;
};

cbuffer ModelMatrixBuffer : register(b0)
{
    matrix worldMatrix;
};

struct VertexInputType
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    uint matrixIndex : MATRIXINDEX;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOS;
    float2 depth : DEPTH;
    float3 screenPos : SCREENPOS;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraPos : CAMERAPOS;
};

VOut VS_main(VertexInputType input, uint instanceID : SV_InstanceID)
{
    VOut output;
    output.position = mul(float4(input.position, 1.0f), boneTransforms[input.matrixIndex]);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.depth = output.position.zw;

    output.screenPos = output.position.xyw;

    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    output.normal = mul(float4(input.normal, 0.0f), boneTransforms[input.matrixIndex]).xyz; // TODOXX TODOLH ANIM WARNING!! osäker på hur detta funkar!!!!!!!!
    output.normal = mul(float4(output.normal, 0.0f), worldMatrix).xyz;
    output.texCoord = input.texCoord;
    output.cameraPos = cameraPosition;
    return output;
}