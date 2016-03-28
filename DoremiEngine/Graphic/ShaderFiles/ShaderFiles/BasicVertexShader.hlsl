#define MAX_NUMBER_OF_INSTANCES 100

cbuffer CameraMatrixBuffer : register(b1)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix inverseProjection;
    float3 cameraPosition;
    float pad;
};

struct WorldMatrixStruct
{
    matrix worldMatrix;
    matrix invTransWorldMatrix;
};
cbuffer ModelMatrixBuffer : register(b0)
{
    WorldMatrixStruct worldMatrices[MAX_NUMBER_OF_INSTANCES];
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
    float2 depth : DEPTH;
    float3 screenPos : SCREENPOS;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 cameraSpacePos : cameraSpacePos;
    float3 viewDir : VIEWDIR;
    float fogFactor : FOG;
};

VOut VS_main(VertexInputType input, uint instanceID : SV_InstanceID)
{
    VOut output;
    output.position = mul(float4(input.position, 1.0f), worldMatrices[instanceID].worldMatrix);
    float4 cameraSpacePos = output.position;
    cameraSpacePos = mul(cameraSpacePos, viewMatrix); //flika in där här coz den behöver skiten ovanför, vill inte göra den beräkningen twice
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depth = output.position.zw;

    output.screenPos = output.position.xyw;

    output.worldPos = mul(float4(input.position, 1.0f), worldMatrices[instanceID].worldMatrix);
    output.normal = mul(float4(input.normal, 0.0f), worldMatrices[instanceID].invTransWorldMatrix).xyz;
    output.texCoord = input.texCoord;
    output.cameraSpacePos = cameraSpacePos;
    output.viewDir = cameraPosition.xyz - output.worldPos.xyz; //cameraPosition är kamerans position medans den andra variabeln är förhållande till kameran
    output.viewDir = normalize(output.viewDir);

    float fogStart = 10.0f;
    float fogEnd = 1250.0f;
    // Calculate linear fog.    
    output.fogFactor = saturate((fogEnd - cameraSpacePos.z) / (fogEnd - fogStart));
    return output;
}