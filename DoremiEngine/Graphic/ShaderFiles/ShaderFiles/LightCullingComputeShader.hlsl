#include "CommonInclude.hlsl"

// Precomputed frustums for the grid.
struct LightGridInfo
{
    uint offset;
    uint value;
};
//StructuredBuffer<Light> Lights : register(t8);
//StructuredBuffer<Frustum> in_Frustums : register(t9);

// Global counter for current index into the light index list.
// "o_" prefix indicates light lists for opaque geometry while 
// "t_" prefix indicates light lists for transparent geometry.

SamplerState ObjSamplerState : register(s0);
 
StructuredBuffer<Frustum> in_Frustums : register(t0);
// The depth from the screen space texture.
Texture2D DepthTextureVS : register(t1);


RWStructuredBuffer<uint> o_LightIndexCounter : register(u0);
RWStructuredBuffer<uint> t_LightIndexCounter : register(u1);

RWStructuredBuffer<uint> o_LightIndexList : register(u2);
RWStructuredBuffer<uint> t_LightIndexList : register(u3);
//RWTexture2D<uint2> o_LightGrid : register(u5);
//RWTexture2D<uint2> t_LightGrid : register(u6);
RWStructuredBuffer<LightGridInfo> o_LightGrid : register(u4);
RWStructuredBuffer<LightGridInfo> t_LightGrid : register(u5);
RWTexture2D<float4> backbuffer : register(u6);

cbuffer LightInfo : register(b1)
{
    Light lights[NUM_LIGHTS];
};

groupshared uint uMinDepth;
groupshared uint uMaxDepth;
groupshared Frustum GroupFrustum;

// Opaque geometry light lists.
groupshared uint o_LightCount;
groupshared uint o_LightIndexStartOffset;
groupshared uint o_LightList[1024];     //Max 1024 lights per thread group. Should never use all of them.

// Transparent geometry light lists.
groupshared uint t_LightCount;
groupshared uint t_LightIndexStartOffset;
groupshared uint t_LightList[1024];


// Add the light to the visible light list for opaque geometry.
//Change to pass group-shared variabes if possible
void o_AppendLight(uint lightIndex)
{
    uint index; // Index into the visible lights array.
    InterlockedAdd(o_LightCount, 1, index);
    if (index < 1024)
    {
        o_LightList[index] = lightIndex;
    }
}

// Add the light to the visible light list for transparent geometry.
void t_AppendLight(uint lightIndex)
{
    uint index; // Index into the visible lights array.
    InterlockedAdd(t_LightCount, 1, index);
    if (index < 1024)
    {
        t_LightList[index] = lightIndex;
    }
}


[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CS_main(ComputeShaderInput input)
{
    //TODORK send as parameter
    uint3 numThreadGroups = uint3(50, 50, 1);
    uint3 numThreads = uint3(800, 800, 1);

    // Calculate min & max depth in threadgroup / tile.
    int2 texCoord = input.dispatchThreadID.xy;
    float fDepth = DepthTextureVS.Load(int3(texCoord, 0)).r;


    

    uint uDepth = asuint(fDepth);   // reinterpret as int since atomic operations cannot be performed on floats

    if (input.groupIndex == 0) // Avoid contention by other threads in the group.
    {
        if (input.groupID.x == 0 && input.groupID.y == 0)
        {
            o_LightIndexCounter[0] = 0;
            t_LightIndexCounter[0] = 0;
        }
        uMinDepth = 0xffffffff;
        uMaxDepth = 0;
        o_LightCount = 0;
        t_LightCount = 0;
        GroupFrustum = in_Frustums[input.groupID.x + (input.groupID.y * numThreadGroups.x)];
    }

    GroupMemoryBarrierWithGroupSync();
    InterlockedMin(uMinDepth, uDepth);  //automatically update max and min depth
    InterlockedMax(uMaxDepth, uDepth);

    GroupMemoryBarrierWithGroupSync();

    float fMinDepth = asfloat(uMinDepth);
    float fMaxDepth = asfloat(uMaxDepth);

    //render depth to back buffer
    //backbuffer[input.dispatchThreadID.xy] = float4(fMinDepth, 0, fMaxDepth, 1);
    //backbuffer[input.dispatchThreadID.xy] = float4(fMinDepth, fMinDepth, fMinDepth, 1); b
    //float fMinDepth = 0;
    //float fMaxDepth = 100;

    // Convert depth values to view space.
    float minDepthVS = ClipToView(float4(0, 0, fMinDepth, 1)).z;
    float maxDepthVS = ClipToView(float4(0, 0, fMaxDepth, 1)).z;
    float nearClipVS = ClipToView(float4(0, 0, 0, 1)).z;

    // Clipping plane for minimum depth value 
    // (used for testing lights within the bounds of opaque geometry).
    Plane minPlane = { float3(0, 0, -1), -minDepthVS };

    int i = 0;
    // Cull lights
    // Each thread in a group will cull 1 light until all lights have been culled.  
    for (i = input.groupIndex; i < NUM_LIGHTS; i += BLOCK_SIZE * BLOCK_SIZE)
    {
        if (lights[i].enabled)
        {
            Light light = lights[i];

            switch (light.type)
            {
            case 3: //pointlight
                Sphere sphere = { mul(light.position, viewMatrix).xyz, light.intensity * 40.f}; //TODORK change intensity to light range
                if (SphereInsideFrustum(sphere, GroupFrustum, nearClipVS, maxDepthVS))
                {
                    // Add light to light list for transparent geometry.
                    t_AppendLight(i);

                    if (!SphereInsidePlane(sphere, minPlane))
                    {
                        // Add light to light list for opaque geometry.
                        o_AppendLight(i);
                    }
                }


                break;
            case 2: //spot light

                float coneRadius = tan(radians(light.coneAngle)) * light.intensity * 50; //TODORK change intensity to light range
                Cone cone = { light.position.xyz, light.intensity, light.direction.xyz, coneRadius };
                if (ConeInsideFrustum(cone, GroupFrustum, nearClipVS, maxDepthVS))
                {
                    // Add light to light list for transparent geometry.
                    t_AppendLight(i);

                    if (!ConeInsidePlane(cone, minPlane))
                    {
                        // Add light to light list for opaque geometry.
                        o_AppendLight(i);
                    }
                }

                break;
            case 1: //directional light

                // Directional lights always get added to our light list.
                // (Hopefully there are not too many directional lights!)
                t_AppendLight(i);
                o_AppendLight(i);

                break;
            }
        }
    }


    // Update global memory with visible light buffer.
    // First update the light grid (only thread 0 in group needs to do this)
    if (input.groupIndex == 0)
    {
        // Update light grid for opaque geometry.
        InterlockedAdd(o_LightIndexCounter[0], o_LightCount, o_LightIndexStartOffset);
        LightGridInfo lightGridInfo;
        lightGridInfo.offset = o_LightIndexStartOffset;
        lightGridInfo.value = o_LightCount;
        //o_LightGrid[input.groupID.xy] = uint2(o_LightIndexStartOffset, o_LightCount);
        uint index = input.groupID.x + (input.groupID.y * numThreadGroups.x);
        o_LightGrid[index] = lightGridInfo;

        // Update light grid for transparent geometry.
        InterlockedAdd(t_LightIndexCounter[0], t_LightCount, t_LightIndexStartOffset);
        lightGridInfo.offset = t_LightIndexStartOffset;
        lightGridInfo.value = t_LightCount;
        //t_LightGrid[input.groupID.xy] = uint2(t_LightIndexStartOffset, t_LightCount);
        index = input.groupID.x + (input.groupID.y * numThreadGroups.x);
        t_LightGrid[index] = lightGridInfo;


        
    }

    GroupMemoryBarrierWithGroupSync();

    // Now update the light index list (all threads).
    // For opaque geometry.
    for (i = input.groupIndex; i < o_LightCount; i += BLOCK_SIZE * BLOCK_SIZE)
    {
        o_LightIndexList[o_LightIndexStartOffset + i] = o_LightList[i];
    }
    // For transparent geometry.
    for (i = input.groupIndex; i < t_LightCount; i += BLOCK_SIZE * BLOCK_SIZE)
    {
        t_LightIndexList[t_LightIndexStartOffset + i] = t_LightList[i];
    }
    uint a = o_LightIndexList[0];
    //render depth to back buffer
    //backbuffer[input.dispatchThreadID.xy] = float4(((float)o_LightCount) / 7.0f, ((float)o_LightCount)/ 7.0f, ((float)o_LightCount) / 7.0f,1.0f);
}