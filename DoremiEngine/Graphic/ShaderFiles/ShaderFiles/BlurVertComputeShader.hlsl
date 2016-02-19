#include "CommonInclude.hlsl"
//#define BLUR_SIZE 5
#define BLOCK_SIZE 256

Texture2D glowmap : register (t0);
Texture2D scene : register (t1);
RWTexture2D<float4> output : register (u0);

groupshared float4 gCache[BLOCK_SIZE*BLOCK_SIZE];
cbuffer cbFixed
{
    static const int BLUR_SIZE = 5;
};

cbuffer cbWeights
{
    static const float gWeights[11] =
    {
        0.027f, 0.055f, 0.083f, 0.1f, 0.139f, 0.17f, 0.139f, 0.1f, 0.083f, 0.055f, 0.027f,
    };
};



[numthreads(1, BLOCK_SIZE, 1)]
void CS_main(ComputeShaderInput input)
{
    float2 index2d = input.dispatchThreadID.xy;
    float index = index2d.x + (index2d.y * SCREEN_WIDTH);

    // Wait for all threads to finish.
    GroupMemoryBarrierWithGroupSync();

    ///// BLUR HORIZONAL

    float4 blurColor = float4(0, 0, 0, 0);

    [unroll]
    for (int i = -BLUR_SIZE; i < BLUR_SIZE; i++)
    {
        int k = index2d.y + i;
        float2 texcoords = float2(index2d.x, k);
        blurColor += gWeights[i + BLUR_SIZE] * glowmap[texcoords];
    }

    output[index2d.xy] = saturate(scene[index2d] + blurColor);
    //output[index2d.xy] = scene[index2d];

}
