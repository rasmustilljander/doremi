#pragma once

const int MAX_NAME_SIZE = 100;

struct NameMaxNameStruct
{
    char name[100];
};

struct Float2
{
    float u, v;
    Float2() {}
    Float2(float fu, float fv)
    {
        u = fu;
        v = fv;
    }

    void Set(float fu, float fv)
    {
        u = fu;
        v = fv;
    }
};

struct Float3
{
    float x, y, z;
    Float3()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    Float3(float fx, float fy, float fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }

    void Set(float fx, float fy, float fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }
};

struct Float4
{
    float x, y, z, w;
    Float4()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 1; // hmm?
    }
    Float4(float fx, float fy, float fz, float fw)
    {
        x = fx;
        y = fy;
        z = fz;
        w = fw;
    }

    void Set(float fx, float fy, float fz, float fw)
    {
        x = fx;
        y = fy;
        z = fz;
        w = fw;
    }
};

struct Int3
{
    int x, y, z;
    Int3() {}
    Int3(int fx, int fy, int fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }

    void Set(int fx, int fy, int fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }
};
struct Int2
{
    int u, v;
    Int2() {}
    Int2(int fu, int fv)
    {
        u = fu;
        v = fv;
    }

    void Set(int fu, int fv)
    {
        u = fu;
        v = fv;
    }
};

struct Vertex
{
    Float3 pos, nor;
    Float2 uv;

    Vertex() {}
    ~Vertex() {}
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float cu, float cv)
    {
        pos.Set(px, py, pz);
        nor.Set(nx, ny, nz);
        uv.Set(cu, cv);
    }
};
struct IndexV
{
    int posI, norI, uvI;

    IndexV() {}
    ~IndexV() {}
    IndexV(int p, int n, int u)
    {
        posI = p;
        norI = n;
        uvI = u;
    }
};

struct CustomAttributes
{
    bool isRendered;
    bool isBBox;
    bool isCollider;
    bool isInteractable;
    int checkPointID;
    int spawnPointID;
    int startOrEndPoint;
    bool isAIground;

    bool frequencyAffected;
    float interactableRange;
    Float3 interactableStartPos;
    Float3 interactableEndPos;
    Float2 interactableOffset;
    float interactableStartSpeed;
    float interactableEndSpeed;
    bool isPotentialFieldCollidable;
    int potentialFieldID;
    int potentialFieldNeighbour1;
    int potentialFieldNeighbour2;
    int potentialFieldNeighbour3;
    int potentialFieldNeighbour4;
    int potentialFieldNeighbour5;

    bool isSpawner;
    int spawnMax;
    int spawnMaxAlive;
    int spawnTypeBlueprint;
    float spawnFrequency;

    bool isStatic;
    int physicsType;

    int typeBlueprint;

    bool isDangerous;
};
struct TransformData
{
    Float3 pos;
    Float4 rot;
    Float3 scale;
    CustomAttributes attributes;
};

struct CameraData
{
    int isOrtho;
    float target[3];
    float upVector[3];
    float rightVector[3];
    float hAngle; // hor-FOV
};

struct MeshData
{
    int nrPos, nrNor, nrUV, nrI;
    int triangleCount;
    Float3* positions;
    Float3* normals;
    Float2* uvs;

    int* indexPositions;
    int* indexNormals;
    int* indexUVs;
    // int *indexTriangles;
    int* trianglesPerFace;
    // Material *material; //pekar på ett specifikt material i en vektor av material
    ~MeshData()
    {
        delete(positions);
        delete(normals);
        delete(uvs);

        delete(indexPositions);
        delete(indexNormals);
        delete(indexUVs);
        delete(trianglesPerFace);
    }
};

struct MaterialData
{
    MaterialData()
    {
        mapMasks = 0;
        diffuse = 0;
        color[0] = color[1] = color[2] = 0.5f;
        ambColor[0] = ambColor[1] = ambColor[2] = 0.0f;
        specColor[0] = specColor[1] = specColor[2] = 0.0f;
        specCosine = specEccentricity = specRollOff = 0;
    }
    int mapMasks;
    float diffuse;
    float color[3];
    float ambColor[3];
    float specColor[3];
    float specCosine;
    float specEccentricity;
    float specRollOff;
};

struct LightData
{
    int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
    int decayType; // 0 = none, 1 = linear, 2 = quadratic (l/d**v)
    float intensity;
    Float3 colorDiffuse;
    Float3 direction;
    float dropOff;
    float coneAngle;
    float penumAgle;
};

struct LightCBufferData
{ // ett ljus
    int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
    Float3 attenuation; // 0 = none, 1 = linear, 2 = quadratic (l/d**v)
    float intensity;
    Float3 colorDiffuse;
    float coneAngle;
    Float3 direction;
    float penumAgle;
    Float3 position;
};