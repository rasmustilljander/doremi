#include "CommonInclude.hlsl"
//#define BLUR_SIZE 5
#define BLOCK_SIZE 32

Texture2D scene : register (t0);
Texture2D glowmap : register (t1);
//RWTexture2D<float4> halfblur : register (u2);
RWTexture2D<float4> output : register (u0);
RWStructuredBuffer<float4> horizonalValues : register(u1);

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



[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CS_main(ComputeShaderInput input)
{
    float2 index2d = input.dispatchThreadID.xy;
    float index = index2d.x + (index2d.y * SCREEN_WIDTH);

    //gCache[index] = scene[input.dispatchThreadID.xy];

    // Wait for all threads to finish.
    GroupMemoryBarrierWithGroupSync();

    ///// BLUR HORIZONAL

    float4 blurColor = float4(0, 0, 0, 0);

    [unroll]
    for (int i = -BLUR_SIZE; i < BLUR_SIZE; i++)
    {
        int k = index2d.x + i;
        float2 texcoords = float2(k, index2d.y);
        blurColor += gWeights[i + BLUR_SIZE] * glowmap[texcoords];
    }


    horizonalValues[index] = blurColor;

    GroupMemoryBarrierWithGroupSync();
    
    ///////// BLUR VERTICAL
    
    blurColor = float4(0, 0, 0, 0);
    
    for (int i = -BLUR_SIZE; i <= BLUR_SIZE; ++i)
    {
        //int k = index2d.y + BLUR_SIZE + i;
        int k = index2d.y + i;
        float index2 = index2d.x + k * SCREEN_WIDTH;
        blurColor += gWeights[i + BLUR_SIZE] * horizonalValues[index2];
    }
    output[index2d.xy] = saturate(scene[index2d.xy] + blurColor);

}
