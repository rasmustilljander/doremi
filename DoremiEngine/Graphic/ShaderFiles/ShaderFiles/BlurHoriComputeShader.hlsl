//#include "CommonInclude.hlsl"
//#define BLOCK_SIZE 256//
//
//Texture2D glowmap : register (t0);
//RWTexture2D<float4> output : register (u0);
////
//cbuffer cbFixed
//{
//    static const int BLUR_SIZE = 5;
//};
//
//cbuffer cbWeights
//{
//    static const float gWeights[11] =
//    {
//        //0.027f, 0.055f, 0.083f, 0.1f, 0.139f, 0.17f, 0.139f, 0.1f, 0.083f, 0.055f, 0.027f,
//        0.08f, 0.08f, 0.15f, 0.15f, 0.25f, 0.3f, 0.25f, 0.15f, 0.15f, 0.08f, 0.08f,
//    };
//};
//
//
//groupshared float4 gCache[BLOCK_SIZE + BLUR_SIZE * 2];
//
//[numthreads(BLOCK_SIZE, 1, 1)]
//void CS_main(ComputeShaderInput input)
//{
//    float2 index2d = input.dispatchThreadID.xy;
//    float index = index2d.x + (index2d.y * SCREEN_WIDTH);
//
//    // Wait for all threads to finish.
//    GroupMemoryBarrierWithGroupSync();
//
//    ///// BLUR HORIZONAL
//
//    float4 blurColor = float4(0, 0, 0, 0);
//
//    [unroll]
//    for (int i = -BLUR_SIZE; i < BLUR_SIZE; i++)
//    {
//        int k = index2d.x + i;
//        float2 texcoords = float2(k, index2d.y);
//        blurColor += gWeights[i + BLUR_SIZE] * glowmap[texcoords];
//    }
//
//    output[index2d.xy] = blurColor;
//
//}


#include "CommonInclude.hlsl"
#define BLOCK_SIZE 256

Texture2D glowmap : register (t0);
Texture2D color : register(t1);
RWTexture2D<float4> output : register (u0);

cbuffer cbFixed
{
    static const int BLUR_SIZE = 5;
};

cbuffer cbWeights
{
    static const float gWeights[11] =
    {
        //0.027f, 0.055f, 0.083f, 0.1f, 0.139f, 0.17f, 0.139f, 0.1f, 0.083f, 0.055f, 0.027f,
        0.08f, 0.08f, 0.15f, 0.15f, 0.25f, 0.3f, 0.25f, 0.15f, 0.15f, 0.08f, 0.08f,
    };
};


groupshared float4 gCache[BLOCK_SIZE + BLUR_SIZE * 2];

[numthreads(BLOCK_SIZE, 1, 1)]
void CS_main(ComputeShaderInput input)
{
    int2 topLeftBoxID = input.dispatchThreadID.xy * 2;

    // get the edges
    if (input.groupThreadID.x < BLUR_SIZE)
    {
        // clamp out of bound samples
        int x = max(input.dispatchThreadID.x - BLUR_SIZE, 0);

        // Warning only works when the threads match the texture
        gCache[input.groupThreadID.x] = glowmap[int2(x, input.dispatchThreadID.y)];
    }
    if (input.groupThreadID.x >= BLOCK_SIZE - BLUR_SIZE)
    {
        // clamp out of bound samples
        int x = min(input.dispatchThreadID.x + BLUR_SIZE, glowmap.Length.x - 1);

        // Warning only works when the threads match the texture
        gCache[input.groupThreadID.x + 2*BLUR_SIZE] = glowmap[int2(x, input.dispatchThreadID.y)];
    }

    // Get the rest of the samples
    gCache[input.groupThreadID.x + BLUR_SIZE] = glowmap[min(input.dispatchThreadID.xy, glowmap.Length.xy - 1)];

    // Wait for all threads to finish.
    GroupMemoryBarrierWithGroupSync();

    ///// BLUR HORIZONAL
    float4 blurColor = float4(0, 0, 0, 0);

    // i = -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, -> 5 = 9
    //[unroll]
    for (int i = -BLUR_SIZE; i <= BLUR_SIZE; ++i)
    {
        int k = input.groupThreadID.x + BLUR_SIZE + i;
        blurColor += gWeights[i + BLUR_SIZE] * gCache[k];
    }

    output[topLeftBoxID.xy] = blurColor + color[topLeftBoxID.xy];
    output[int2(topLeftBoxID.x + 1, topLeftBoxID.y)] = blurColor + color[int2(topLeftBoxID.x + 1, topLeftBoxID.y)];
    output[int2(topLeftBoxID.x,     topLeftBoxID.y + 1)] = blurColor + color[int2(topLeftBoxID.x, topLeftBoxID.y + 1)];
    output[int2(topLeftBoxID.x + 1, topLeftBoxID.y + 1)] = blurColor + color[int2(topLeftBoxID.x + 1, topLeftBoxID.y + 1)];

    // I did for 4 .. for some reason

}
