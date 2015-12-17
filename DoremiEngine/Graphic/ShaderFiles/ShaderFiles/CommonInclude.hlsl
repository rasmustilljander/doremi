#define NUM_LIGHTS 8;

struct Material
{
    float3 color;
    float diffuse;
    float3 ambColor;
    float specCosine;
    float3 specColor;
    float specEccentricity;
    float3 padding;
    float specRollOff;
};

struct Light
{
    int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
    float3 attenuation;
    float intensity;
    float3 color;
    float coneAngle;
    float3 direction;
    float penumAgle;
    float3 position;
};

struct Plane
{
    float3 N;   // Plane normal.
    float  d;   // Distance to origin.
};

struct Sphere
{
    float3 c;   // Center point.
    float  r;   // Radius.
};

struct Cone
{
    float3 T;   // Cone tip.
    float  h;   // Height of the cone.
    float3 d;   // Direction of the cone.
    float  r;   // bottom radius of the cone.
};

//cbuffer CameraMatrixBuffer : register(b1)
//{
//    matrix viewMatrix;
//    matrix projectionMatrix;
//    matrix inverseProjection;
//    float3 cameraPosition;
//    float pad;
//};

struct Frustum
{
    Plane plane[4];   // left, right, top, bottom frustum planes.
};


//float4 ClipToView(float4 clip)
//{
//    // View space position.
//    //TODORK change to inverseProjection
//    float4 view = mul(projectionMatrix, clip);
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

Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;

    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;

    plane.N = normalize(cross(v0, v2));

    // Compute the distance to the origin using p0.
    plane.d = dot(plane.N, p0);

    return plane;
}