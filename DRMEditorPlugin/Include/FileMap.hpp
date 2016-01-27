#pragma once
#ifndef FILEMAP_H
#define FILEMAP_H


#include <Windows.h>
#include <ostream>
#include <iostream>

#include <maya/MGlobal.h>


enum MessageType
{
    msgAdded = 1,
    msgEdited = 2,
    msgDeleted = 3,
    msgRenamed = 4,
    msgSwitched = 5
};
enum NodeType
{
    nDefaultNode = 0,
    nMesh = 1,
    nTransform = 2,
    nCamera = 3,
    nLight = 4,
    nMaterial = 5
};
enum class bitmask : int
{
    COLORMAP = 0x01,
    GLOWMAP = 0x02,
    SPECMAP = 0x04,
    BUMPMAP = 0x08
};

struct MessageHeader
{
    MessageHeader()
    {
        nodeType = 0;
        messageType = 0;
        msgConfig = 0;
        byteTotal = 0;
        byteSize = 0;
        bytePadding = 0;
    }
    int nodeType;
    int messageType;
    int msgConfig;
    size_t byteTotal;
    size_t byteSize;
    size_t bytePadding;
};
struct MessageInfo
{
    std::string nodeName;
    int nodeType;
    int msgType;
    std::string oldName;
};
struct RenameDeleteInfo
{
    std::string nodeName1;
    std::string nodeName2;
};
struct RenameDeleteMessage
{
    char nodeName1[100];
    char nodeName2[100];
};


struct TransformData
{
    float translation[3];
    float rotation[4];
    float scale[3];
};
struct TransformInfo
{
    std::string nodeName;
    std::string parentName; // om ingen parent sätt till 0 första bokstav fuck off.
    TransformData transformData;
};
struct TransformMessage
{
    char nodeName[100];
    char parentName[100];
    TransformData trData;
};
struct MeshData
{
    int vertCount;
    int normalCount;
    int UVCount;
    int indCount;
    int triCount;
    // int triCountTotal;
    const float* vertices;
    const float* normals;
    float2* uv;
    int* triIndices;
    int* norIndices;
    int* UVIndices;
    int* triPerFace;

    ~MeshData()
    {
        // delete[] this->triPerFace;
    }
};
struct MeshInfo
{
    std::string nodeName;
    std::string transformName;
    std::string materialName;
    int meshID;
    int materialID;
    MeshData meshData;
};
struct MeshMessage
{
    char nodeName[100];
    char transformName[100];
    char materialName[100];
    int meshID;
    int materialID;
    MeshData meshData;
};
struct CameraData
{
    int isOrtho;
    float target[3];
    float upVector[3];
    float rightVector[3];
    float hAngle;
};
struct CameraInfo
{
    std::string nodeName;
    std::string transformName;
    CameraData camData;
};
struct CameraMessage
{
    char nodeName[100];
    char transformName[100];
    CameraData camData;
};
struct MaterialData
{
    MaterialData()
    {
        mapMasks = 0;
        diffuse = 0;
        color[0] = color[1] = color[2] = 0.0f;
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
struct MaterialInfo
{
    std::string nodeName;
    std::string diffuseTexturePath;
    std::string glowTexturePath;
    std::string specTexturePath;
    std::string bumpTexturePath;
    int type;
    MaterialData matData;
};
struct MaterialMessage
{
    char nodeName[100];
    char diffuseTexturePath[100];
    char glowTexturePath[100];
    char specTexturePath[100];
    char bumpTexturePath[100];
    int type;
    MaterialData matData;
};
struct LightData
{
    LightData()
    {
        type = 0;
        decayType = 0;
        intensity = 0;
        colorDiffuse[0] = colorDiffuse[1] = colorDiffuse[2] = 0.5f;
        direction[0] = direction[1] = direction[2] = 0.0f;
        dropOff = 1.0f;
        coneAngle = 0.0f;
        penumAgle = 0.0f;
    }
    int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
    int decayType; // 0 = none, 1 = linear, 2 = quadratic (l/d**v)
    float intensity;
    float colorDiffuse[3];
    float direction[3];
    float dropOff;
    float coneAngle;
    float penumAgle;
};
struct LightInfo
{
    std::string nodeName;
    std::string transformName;
    LightData lightData;
};
struct LightMessage
{
    char nodeName[100];
    char transformName[100];
    LightData lightData;
};

class Mutex
{
private:
    HANDLE handle_;

public:
    Mutex(const char* name = "__my_mutex__")
    {
        // handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));
    }
    ~Mutex() { ReleaseMutex(handle_); }
    void Create(const char* name = "__my_mutex__")
    {
        // handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));

        handle_ = OpenMutex(MUTEX_ALL_ACCESS, FALSE, (LPWSTR)(name)); //(LPWSTR)(name) TEXT("__info_Mutex__")
    }
    bool Lock(DWORD milliseconds = 10000000000000)
    {
        DWORD check;
        check = WaitForSingleObject(handle_, milliseconds);
        // Sleep(milliseconds); //check returneras direkt så vänta ut skiten oxå??
        if(check == WAIT_ABANDONED)
        {
            return false; // didnt get mutex
        }

        return true; // got mutex
    }
    void Unlock() { ReleaseMutex(handle_); }
};

class FileMapping
{
public:
    FileMapping();
    ~FileMapping();
    void CreateFileMaps(bool debug = false);
    void SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize);
    void GetFilemapInfoValues();
    void PrintFileMapInfo(bool isPost);
    static void printInfo(MString instr);
    static void printError(MString instr);
    static void printWarning(MString instr);
    MessageHeader createHeaderTransform(MessageInfo& msginfo, TransformInfo& tInfo);
    MessageHeader createHeaderMesh(MessageInfo& msginfo, MeshInfo& minfo);
    MessageHeader createHeaderCamera(MessageInfo& msginfo, CameraInfo& cInfo);
    MessageHeader createHeaderMaterial(MessageInfo& msginfo, MaterialInfo& mInfo);
    MessageHeader createHeaderLight(MessageInfo& msginfo, LightInfo& lInfo);

    MeshMessage createMessageMesh(MessageInfo& msginfo, MeshInfo& mInfo);
    TransformMessage createMessageTransform(MessageInfo& msginfo, TransformInfo& tInfo);
    CameraMessage createMessageCamera(MessageInfo& msginfo, CameraInfo& cInfo);
    MaterialMessage createMessageMaterial(MessageInfo& msginfo, MaterialInfo& mInfo);
    LightMessage createMessageLight(MessageInfo& msgInfo, LightInfo& lInfo);
    size_t makeMultiple(size_t size, size_t multiple);
    bool tryWriteTransform(MessageInfo& msg, TransformInfo& tinfo);
    bool tryWriteMesh(MessageInfo& msg, MeshInfo& minfo);
    bool tryWriteCamera(MessageInfo& msg, CameraInfo& cinfo);
    bool tryWriteMaterial(MessageInfo& msg, MaterialInfo& minfo);
    bool tryWriteLight(MessageInfo& msg, LightInfo& linfo);

    int findWriteConfig(MessageHeader& hdr);

    bool writeTransform(MessageHeader& hdr, TransformMessage& tdata, int config);
    bool writeMesh(MessageHeader& hdr, MeshMessage& mdata, int config);
    bool writeCamera(MessageHeader& hdr, CameraMessage& cdata, int config);
    bool writeMaterial(MessageHeader& hdr, MaterialMessage& mdata, int config);
    bool writeLight(MessageHeader& hdr, LightMessage& ldata, int config);

    bool tryWriteRenameDelete(MessageInfo& info, RenameDeleteInfo& msg);
    MessageHeader createHeaderRenameDelete(MessageInfo& msginfo);
    RenameDeleteMessage createMessageRenameDelete(MessageInfo& msgInfo, RenameDeleteInfo& info);
    bool writeNodeRenamedDelete(MessageHeader& hdr, RenameDeleteMessage& msg, int config);


    std::string GetLastErrorAsString();

    bool debug;


private:
    struct FilemapInfo
    {
        size_t head_ByteOffset; // offset in bytes from beginning of the shared memory
        size_t tail_ByteOffset; // offset in bytes from beginning of the shared memory
        size_t non_accessmemoryOffset; // memory in beginning of file thats no touchy, so that head and tail won't get to each other
        // size_t totalConsumers;
        size_t messageFilemap_Size;

        FilemapInfo()
        {
            head_ByteOffset = 0;
            tail_ByteOffset = 0;
            non_accessmemoryOffset = 0;
            // totalConsumers = 0;
            messageFilemap_Size = 0; // storleken på filemapen med meddelanden
        }
    };

    FilemapInfo fileMapInfo;
    HANDLE hMessageFileMap = nullptr;
    ;
    LPVOID mMessageData = nullptr;
    ;
    unsigned int mSize;
    HANDLE hInfoFileMap = nullptr;
    ;
    LPVOID mInfoData = nullptr;
    unsigned int mInfoSize = 256;

    size_t localHead = 0;
    size_t localTail = 0;
    Mutex mutexInfo;

    size_t memoryPadding;
    bool headerFit;
    // size_t _headerSize;

    MessageHeader messageHeader;

    // TransformMessage *transformMessage;
    // CameraMessage *cameraMessage;
    // MeshMessage *meshMessage;
    // MaterialMessage *materialMessage;
    // LightMessage *lightMessage;
};


#endif