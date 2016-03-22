#include "CommonInclude.hlsl"
#define BLOCK_SIZE 256


Texture2D glowmap : register (t0);
Texture2D scene : register (t1);
RWTexture2D<float4> output : register (u0);
SamplerState glowSampler : register (s0);

// This goes first

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

groupshared float4 gCache[BLOCK_SIZE + 2 * BLUR_SIZE];

[numthreads(1, BLOCK_SIZE, 1)]
void CS_main(ComputeShaderInput input)
{
    float2 uvDimensions = float2(1.0f/ Resolution.x, 1.0f/ Resolution.y);

    // get the edges
    if (input.groupThreadID.y < BLUR_SIZE)
    {
        // clamp out of bound samples
        int y = max(input.dispatchThreadID.y - BLUR_SIZE, 0);

        // Warning only works when the threads match the texture
        gCache[input.groupThreadID.y] = glowmap.SampleLevel(glowSampler, float2(input.dispatchThreadID.x* uvDimensions.x, y*uvDimensions.y), 0);
    }
    if (input.groupThreadID.y >= BLOCK_SIZE - BLUR_SIZE)
    {
        // clamp out of bound samples
        int y = min(input.dispatchThreadID.y + BLUR_SIZE, glowmap.Length.y - 1);

        // Warning only works when the threads match the texture
        gCache[input.groupThreadID.y+ 2 * BLUR_SIZE] = glowmap.SampleLevel(glowSampler, float2(input.dispatchThreadID.x*uvDimensions.x, y*uvDimensions.y), 0);
    }

    // Get the rest of the samples
    gCache[input.groupThreadID.y + BLUR_SIZE] = glowmap.SampleLevel(glowSampler, uvDimensions * min(input.dispatchThreadID.xy, glowmap.Length.xy - 1), 0);

    // Wait for all threads to finish.
    GroupMemoryBarrierWithGroupSync();

    ///// BLUR HORIZONAL
    float4 blurColor = float4(0, 0, 0, 0);

    // i = -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, -> 5 = 9
    //[unroll]
    for (int i = -BLUR_SIZE; i <= BLUR_SIZE; ++i)
    {
        int k = input.groupThreadID.y + BLUR_SIZE + i;
        blurColor += gWeights[i + BLUR_SIZE] * gCache[k];
    }

    output[input.dispatchThreadID.xy] = blurColor;

}
