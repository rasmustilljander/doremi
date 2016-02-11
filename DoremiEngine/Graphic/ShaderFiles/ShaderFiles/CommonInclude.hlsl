#define NUM_LIGHTS 30
#define BLOCK_SIZE 16
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

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
    int enabled;
    float3 pad;
};

struct ComputeShaderInput
{
    uint3 groupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

//StructuredBuffer<Light> Lights : register(t8);

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

cbuffer CameraMatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix inverseProjection;
    float3 cameraPosition;
    float pad;
};

struct Frustum
{
    Plane plane[4];   // left, right, top, bottom frustum planes.
};


float4 ClipToView(float4 clip)
{
    // View space position.
    //TODORK change to inverseProjection
    float4 view = mul(clip, inverseProjection);
    // Perspecitive projection.
    view = view / view.w;

    return view;
}

float4 ScreenToView(float4 screen)
{
    float2 ScreenDimensions = float2(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Convert to normalized texture coordinates
    float2 texCoord = screen.xy / ScreenDimensions;

    // Convert to clip space
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

    //return ClipToView(clip);
    return ClipToView(clip);
}

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

// Check to see if a sphere is fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsidePlane(Sphere sphere, Plane plane)
{
    float res = dot(plane.N, sphere.c) - plane.d;
    return res < -sphere.r;
}

// Check to see of a light is partially contained within the frustum.
// Set result to false if point is NOT contained in frustum
bool SphereInsideFrustum(Sphere sphere, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

    // First check depth
    // Note: Here, the view vector points in the -Z axis so the 
    // far depth value will be approaching -infinity.
    //TESTA ÄNDRA < & >
    if (sphere.c.z + sphere.r < zNear || sphere.c.z - sphere.r > zFar)
    {
        // TODORK return false as soon as you know it's outside
        result = false;
    }

    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (SphereInsidePlane(sphere, frustum.plane[i]))
        {
            result = false;
        }
    }
    return result;
}


// Check to see if a point is fully behind (inside the negative halfspace of) a plane.
bool PointInsidePlane(float3 p, Plane plane)
{
    return dot(plane.N, p) - plane.d < 0;
}

// Check to see if a cone if fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool ConeInsidePlane(Cone cone, Plane plane)
{
    // Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross(cross(plane.N, cone.d), cone.d);
    float3 Q = cone.T + cone.d * cone.h - m * cone.r;

    // The cone is in the negative halfspace of the plane if both
    // the tip of the cone and the farthest point on the end of the cone to the 
    // positive halfspace of the plane are both inside the negative halfspace 
    // of the plane.
    return PointInsidePlane(cone.T, plane) && PointInsidePlane(Q, plane);
}

bool ConeInsideFrustum(Cone cone, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

    Plane nearPlane = { float3(0, 0, -1), -zNear };
    Plane farPlane = { float3(0, 0, 1), zFar };

    // First check the near and far clipping planes.
    if (ConeInsidePlane(cone, nearPlane) || ConeInsidePlane(cone, farPlane))
    {
        result = false;
    }

    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (ConeInsidePlane(cone, frustum.plane[i]))
        {
            result = false;
        }
    }
    return result;
}