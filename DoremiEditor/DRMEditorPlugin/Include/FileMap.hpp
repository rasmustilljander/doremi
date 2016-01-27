#pragma once
#ifndef FILEMAP_H
#define FILEMAP_H

#include <Windows.h>
#include <ostream>
#include <iostream>

#include <maya/MGlobal.h>
#include <DoremiEditor/Core/Include/MessageHeader.hpp>
#include <DoremiEditor/Core/Include/CameraMessage.hpp>
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>
#include <DoremiEditor/Core/Include/MeshMessage.hpp>
#include <DoremiEditor/Core/Include/TransformMessage.hpp>
#include <DoremiEditor/Core/Include/MeshData.hpp>
#include <DoremiEditor/Core/Include/TransformData.hpp>
#include <DoremiEditor/Core/Include/CameraData.hpp>
#include <DoremiEditor/Core/Include/MaterialData.hpp>


using namespace DoremiEditor::Core;

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

struct TransformInfo
{
    std::string nodeName;
    std::string parentName; // om ingen parent sätt till 0 första bokstav fuck off.
    TransformData transformData;
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
struct CameraInfo
{
    std::string nodeName;
    std::string transformName;
    CameraData cameraData;
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