#include "CommonInclude.hlsl"


//cbuffer CameraMatrixBuffer : register(b0)
//{
//    matrix viewMatrix;
//    matrix projectionMatrix;
//    matrix inverseProjection;
//    float3 cameraPosition;
//    float pad;
//};

//struct FrustumArray
//{
//    Frustum frustum[2500];
//};

//float4 ClipToView(float4 clip)
//{
//    // View space position.
//    float4 view = mul(inverseProjection, clip);
//    // Perspecitive projection.
//    view = view / view.w;
//
//    return view;
//}
//
//float4 ScreenToView(float4 screen)
//{
//    float2 ScreenDimensions = float2(800, 800);
//    // Convert to normalized texture coordinates
//    float2 texCoord = screen.xy / ScreenDimensions;
//
//    // Convert to clip space
//    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
//
//    return ClipToView(clip);
//}

// View space frustums for the grid cells.
RWStructuredBuffer<Frustum> out_Frustums : register(u0);

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CS_main(ComputeShaderInput input)
{
    //TODORK send as parameter
    uint3 numThreadGroups = uint3(50, 50, 1);
    uint3 numThreads = uint3(800, 800, 1);
    // View space eye position is always at the origin.
    const float3 eyePos = float3(0, 0, 0);

    float3 viewSpace[4];
    Frustum frustum;

    viewSpace[0] = ScreenToView(float4(input.dispatchThreadID.xy * BLOCK_SIZE, -1.0f, 1.0f)).xyz;
    viewSpace[1] = ScreenToView(float4(float2(input.dispatchThreadID.x + 1, input.dispatchThreadID.y) * BLOCK_SIZE, -1.0f, 1.0f)).xyz;
    viewSpace[2] = ScreenToView(float4(float2(input.dispatchThreadID.x, input.dispatchThreadID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f)).xyz;
    viewSpace[3] = ScreenToView(float4(float2(input.dispatchThreadID.x + 1, input.dispatchThreadID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f)).xyz;


    // Left plane
    frustum.plane[0] = ComputePlane(eyePos, viewSpace[2], viewSpace[0]);
    // Right plane
    frustum.plane[1] = ComputePlane(eyePos, viewSpace[1], viewSpace[3]);
    // Top plane
    frustum.plane[2] = ComputePlane(eyePos, viewSpace[0], viewSpace[1]);
    // Bottom plane
    frustum.plane[3] = ComputePlane(eyePos, viewSpace[3], viewSpace[2]);

    // Store the computed frustum in global memory.

    if (input.groupThreadID.x == 0 && input.groupThreadID.y == 0)
    {
        uint index = input.groupID.x + (input.groupID.y * numThreadGroups.x);
        out_Frustums[index] = frustum;
    }
}