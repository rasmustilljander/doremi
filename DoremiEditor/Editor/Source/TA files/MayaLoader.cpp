#include "TA files/MayaLoader.h"

/// Engine
// Core
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphics
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/PixelShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/VertexShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
#include <dxgi.h>
#include <d3d11_1.h>


Mutex mutexInfo("__info_Mutex__");
MayaLoader::MayaLoader(UINT screenWidth, UINT screenHeight, const DoremiEngine::Core::SharedContext& p_sharedContext)
    :m_sharedContext(p_sharedContext)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    CreateFileMaps(1024 * 1024 * 10);
    InitVariables();
    InitDX();
    fileHandler = new FileHandler();
    Material* defaultMaterial = new Material();
    materials.push_back(defaultMaterial); // lägg till default material, viktigt den ligger på första platsen
}
MayaLoader::~MayaLoader()
{
    delete(fileHandler);

    UnmapViewOfFile((LPCVOID)mMessageData);
    CloseHandle(hMessageFileMap);

    UnmapViewOfFile((LPCVOID)mInfoData);
    CloseHandle(hInfoFileMap);
}

void MayaLoader::CreateFileMaps(unsigned int messageFilemapSize)
{
    // mutexInfo.Unlock(); //kanske inte den smartaste idéen?
    // messagefilemap
    mSize = messageFilemapSize;
    hMessageFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, (DWORD)0, mSize, (LPCWSTR) "Global\\messageFileMap");

    mMessageData = MapViewOfFile(hMessageFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if(hMessageFileMap == NULL)
    {
        cout << "Couldn't create filemap\n";
    }

    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        cout << "Filemap exists, you get an handle to it!\n";
    }
    else
    {
        cout << "Creating new filemap\n";
    }


    // info filemapen
    hInfoFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, (DWORD)0, mInfoSize, (LPCWSTR) "Global\\infoFileMap");

    mInfoData = MapViewOfFile(hInfoFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if(hInfoFileMap == NULL)
    {
        cout << "Couldn't create infofilemap\n";
    }
    // FilemapInfo fmInfo;
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        cout << "Infofilemap exists, you get an handle to it!\n";
    }
    else // first, sätter de första värdena på filemapinfon
    {
        cout << "Creating new infofilemap\n";
    }


    SetFilemapInfoValues(0, 0, 10, mSize); // storar de i filemapen oxå! sätt negativa värden om man inte vill nått värde ska ändras :)
}
void MayaLoader::InitVariables()
{

    XMStoreFloat4x4(&defaultCameraCBufferData.view, XMMatrixTranspose(XMMatrixIdentity()));
    XMStoreFloat4x4(&defaultCameraCBufferData.projection, XMMatrixTranspose(XMMatrixIdentity()));

    CreateLightCBufferArray();
}

void MayaLoader::SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize)
{

    while(mutexInfo.Lock(1000) == false)
        Sleep(10); // kommer kalla raden under massa onödiga gånger :s

    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    if(headPlacement >= 0) fileMapInfo.head_ByteOffset = headPlacement;
    if(tailPlacement >= 0) fileMapInfo.tail_ByteOffset = tailPlacement;
    if(nonAccessMemoryPlacement >= 0) fileMapInfo.non_accessmemoryOffset = nonAccessMemoryPlacement;
    if(messageFileMapTotalSize > 0) fileMapInfo.messageFilemap_Size = messageFileMapTotalSize;

    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}

void MayaLoader::DrawScene()
{
    // Complete render list
    size_t numObjects = m_renderObjects.size();
    for (size_t i = 0; i < numObjects; i++)
    {
        XMFLOAT3& p = m_renderObjects[i].position;
        XMFLOAT4& o = m_renderObjects[i].orientation;
        XMFLOAT3& s = m_renderObjects[i].scale;
        // Build world matrix
        XMFLOAT4X4 finalMat;
        XMStoreFloat4x4(&finalMat,
            XMMatrixScaling(s.x, s.y, s.z) *
            XMMatrixRotationQuaternion(XMLoadFloat4(&o)) *
            XMMatrixTranslation(p.x, p.y, p.z));
        m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(
            *m_renderObjects[i].mesh, *m_renderObjects[i].material, finalMat);
    }
    // Somewhat ugly fix to ensure we don't try to draw when there's nothing to draw
    if (numObjects > 0)
    {
        // Do fancy DX stuff that i have no idea what they do
        m_rasterizerState->GetRasterizerState();
        m_depthStencilState->GetDepthStencilState();
        m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().DrawCurrentRenderList(
            m_rasterizerState->GetRasterizerState(), m_depthStencilState->GetDepthStencilState());
        // Done drawing?
    }


    //UINT32 vertexSize2 = sizeof(float) * 8;
    //UINT32 offset2 = 0;
    //// set rätt constantbuffers, ljus, kamera och material stuff!
    //UpdateCameraValues(); // updaterar oxå camera cbuffern

    //for(int i = 0; i < allMeshTransforms.size(); i++)
    //{
    //    Mesh* currMesh = allMeshTransforms[i]->mesh;
    //    gDeviceContext->IASetVertexBuffers(0, 1, &currMesh->vertexBuffer, &vertexSize2, &offset2);
    //    gDeviceContext->IASetIndexBuffer(currMesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    //    if(currMesh->material != nullptr)
    //    {
    //        gDeviceContext->PSSetConstantBuffers(1, 1, &currMesh->material->materialCbuffer);

    //        if(currMesh->material->diffuseTextureView != nullptr)
    //        {
    //            gDeviceContext->PSSetShaderResources(0, 1, &currMesh->material->diffuseTextureView);
    //        }

    //        gDeviceContext->PSSetSamplers(0, 1, &wrap_Sampstate);
    //    }
    //    else
    //    {
    //        gDeviceContext->PSSetConstantBuffers(1, 1, &materials[0]->materialCbuffer);
    //    }
    //    // transformdata ligger på plats 0, material på 1, osv
    //    // set transformcbufferns värden, updatesubresource
    //    allMeshTransforms[i]->UpdateCBuffer(); // slå först ihop med parentens värden innan vi updaterar cbuffern
    //    UpdateLightCBufferArray();
    //    gDeviceContext->Draw(currMesh->nrIndecies, 0);

    //    // gDeviceContext->DrawIndexed(currMesh->nrIndecies, 0, 0);
    //}
}

void MayaLoader::TryReadAMessage()
{
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo)); // hämta filemapinfo datan om jag har fått mutexInfo till den

    if(localTail != fileMapInfo.head_ByteOffset)
    { // sålänge consumern inte har hunnit till headern
        // KAN LÄSA!!
        if((localTail + sizeof(MessageHeader)) <= mSize)
        { // headern ligger på denna sidan!
            headerDidFit = true;
            memcpy(&messageHeader, (unsigned char*)mMessageData + localTail, sizeof(MessageHeader)); // läs headern som vanligt
        }
        else
        { // headern ligger på andra sidan, headern får inte plats
            headerDidFit = false;
            memcpy(&messageHeader, (unsigned char*)mMessageData, sizeof(MessageHeader)); // läs headern i början på filen
            localTail = 0; // flytta över hela meddelandet till andra sidan
        }

        switch(messageHeader.nodeType)
        {
            case 0:
                printf("Skumt ID");
                break;
            case 1:
                ReadMesh(messageHeader.messageType);
                break;
            case 2:
                ReadTransform(messageHeader.messageType);
                break;
            case 3:
                ReadCamera(messageHeader.messageType);
                break;
            case 4:
                ReadLight(messageHeader.messageType);
                break;
            case 5:
                ReadMaterial(messageHeader.messageType);
                break;
            default:
                printf("Invalid message ID");
        }
    }
}

void MayaLoader::ReadTransform(int i)
{
    if(i != 3 && i != 4)
    {
        // får bara headern plats eller får oxå meddelandet plats?
        if(headerDidFit == true)
        { // headern ligger som vanligt, alltså där man är, headern får plats
            transformMessage = (TransformMessage*)malloc(transformMessage_MaxSize);
            if(messageHeader.msgConfig == 1)
            { // meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
                memcpy(transformMessage, (unsigned char*)mMessageData, messageHeader.byteSize); // läser i början på filen utan nån offset
                localTail = messageHeader.byteSize + messageHeader.bytePadding;
            }
            else
            { // meddelandet får plats!!
                memcpy(transformMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal + localTail;
            }
        }
        else
        { // headern ligger på andra sidan, headern får inte plats
            if((localTail + messageHeader.byteSize) > mSize)
            { // meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
                cout << "Filemap too small, a message might be to big for the entire filemap";
            }
            else
            { // annars bara läs den rakt av från där denna redan är placerad
                transformMessage = (TransformMessage*)malloc(transformMessage_MaxSize);
                memcpy(transformMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal;
            }
        }
        transformMessage->transformData.pos.z = transformMessage->transformData.pos.z * -1.0f;
        transformMessage->transformData.rot.x = transformMessage->transformData.rot.x * -1.0f;
        transformMessage->transformData.rot.y = transformMessage->transformData.rot.y * -1.0f;
    }
    else
    {
        ReadName();
    }

    if(i == 1)
    {
        TransformAdded(messageHeader, transformMessage);
    }
    else if(i == 2)
    {
        TransformChange(messageHeader, transformMessage); // tar hand om den aktualla meshen
    }
    else if(i == 3)
    {
        TransformDeleted(messageHeader, nameMessage);
    }
    else if(i == 4)
    {
        TransformRenamed(messageHeader, nameMessage);
    }


    // flytta tailen
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    cout << "Move tail!!!!!" << localTail << "\n";
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    fileMapInfo.tail_ByteOffset = localTail;
    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}
void MayaLoader::ReadMaterial(int i)
{
    if(i != 4 && i != 3)
    {
        if(headerDidFit == true)
        { // headern ligger som vanligt, alltså där man är, headern får plats
            materialMessage = (MaterialMessage*)malloc(materialMessage_MaxSize);
            if(messageHeader.msgConfig == 1)
            { // meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
                memcpy(materialMessage, (unsigned char*)mMessageData, messageHeader.byteSize); // läser i början på filen utan nån offset
                localTail = messageHeader.byteSize + messageHeader.bytePadding;
            }
            else
            { // meddelandet får plats!!
                memcpy(materialMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal + localTail;
            }
        }
        else
        { // headern ligger på andra sidan, headern får inte plats
            // messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen
            // att hålla meddelandet

            if((localTail + messageHeader.byteSize) > mSize)
            { // meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
                cout << "Filemap too small, a message might be to big for the entire filemap";
            }
            else
            { // annars bara läs den rakt av från där denna redan är placerad
                materialMessage = (MaterialMessage*)malloc(materialMessage_MaxSize);
                memcpy(materialMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal;
            }
        }
    }
    else
    {
        ReadName();
    }

    if(i == 1)
    {
        MaterialAdded(messageHeader, materialMessage);
    }
    else if(i == 2)
    {
        MaterialChange(messageHeader, materialMessage); // tar hand om den aktualla meshen
    }
    else if(i == 3)
    {
        MaterialDeleted(messageHeader, nameMessage);
    }
    else if(i == 4)
    {
        MaterialRenamed(messageHeader, nameMessage);
    }


    // flytta tailen
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    cout << "Move tail!!!!!" << localTail << "\n";
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    fileMapInfo.tail_ByteOffset = localTail;
    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}
void MayaLoader::ReadMesh(int i)
{
    if(i != 4 && i != 3)
    {
        // får bara headern plats eller får oxå meddelandet plats?
        if(headerDidFit == true)
        { // headern ligger som vanligt, alltså där man är, headern får plats
            meshMessage = (MeshMessage*)malloc(messageHeader.byteTotal);
            if(messageHeader.msgConfig == 1)
            { // meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
                ReadMeshData(0);
                localTail = messageHeader.byteSize + messageHeader.bytePadding;
            }
            else
            { // meddelandet får plats!!
                ReadMeshData(localTail + sizeof(MessageHeader));
                localTail = messageHeader.byteTotal + localTail;
            }
        }
        else
        { // headern ligger på andra sidan, headern får inte plats
            // messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen
            // att hålla meddelandet

            if((localTail + messageHeader.byteSize) >
               mSize) // meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
            {
                cout << "Filemap too small, a message might be to big for the entire filemap";
            }
            else // annars bara läs den rakt av från där denna redan är placerad
            {
                meshMessage = (MeshMessage*)malloc(messageHeader.byteTotal);
                ReadMeshData(sizeof(MessageHeader));
                localTail = messageHeader.byteTotal;
            }
        }
    }
    else
    {
        ReadName();
    }

    if(i == 1)
    {
        MeshAdded(messageHeader, meshMessage);
    }
    else if(i == 2)
    {
        MeshChange(messageHeader, meshMessage); // tar hand om den aktualla meshen
    }
    else if(i == 4)
    {
        MeshRenamed(messageHeader, nameMessage);
    }
    // flytta tailen
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    cout << "Move tail!!!!!" << localTail << "\n";
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    fileMapInfo.tail_ByteOffset = localTail;
    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}
void MayaLoader::ReadMeshData(size_t offSetStart) // läser vertis data och liknande, all data som är dynamisk
{
    meshMessage->meshData = new MeshData();

    UINT offset = 0;

    memcpy(meshMessage->objectName, (unsigned char*)mMessageData + offSetStart, sizeof(char) * MAX_NAME_SIZE);
    offset += (sizeof(char) * MAX_NAME_SIZE);

    memcpy(&meshMessage->nrOfTransforms, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int));
    offset += sizeof(int);

    meshMessage->transformNames = new NameMaxNameStruct[meshMessage->nrOfTransforms];
    for(int i = 0; i < meshMessage->nrOfTransforms; i++)
    {
        NameMaxNameStruct tempNameStruct;
        memcpy(tempNameStruct.name, (unsigned char*)mMessageData + offSetStart + offset, sizeof(char) * MAX_NAME_SIZE);
        meshMessage->transformNames[i] = tempNameStruct;
        offset += (sizeof(char) * MAX_NAME_SIZE);
    }

    memcpy(meshMessage->materialName, (unsigned char*)mMessageData + offSetStart + offset, sizeof(char) * MAX_NAME_SIZE);
    offset += (sizeof(char) * MAX_NAME_SIZE);

    memcpy(&meshMessage->meshID, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&meshMessage->materialID, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(meshMessage->meshData, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * 5);
    offset += sizeof(int) * 5;

    // allokera minne till att variabler!
    meshMessage->meshData->positions = new Float3[meshMessage->meshData->nrPos];
    meshMessage->meshData->normals = new Float3[meshMessage->meshData->nrNor];
    meshMessage->meshData->uvs = new Float2[meshMessage->meshData->nrUV];

    meshMessage->meshData->indexPositions = new int[meshMessage->meshData->nrI];
    meshMessage->meshData->indexNormals = new int[meshMessage->meshData->nrI];
    meshMessage->meshData->indexUVs = new int[meshMessage->meshData->nrI];
    meshMessage->meshData->trianglesPerFace = new int[meshMessage->meshData->triangleCount];

    memcpy(meshMessage->meshData->positions, (unsigned char*)mMessageData + offSetStart + offset, sizeof(Float3) * meshMessage->meshData->nrPos);
    offset += sizeof(Float3) * meshMessage->meshData->nrPos;
    memcpy(meshMessage->meshData->normals, (unsigned char*)mMessageData + offSetStart + offset, sizeof(Float3) * meshMessage->meshData->nrNor);
    offset += sizeof(Float3) * meshMessage->meshData->nrNor;
    memcpy(meshMessage->meshData->uvs, (unsigned char*)mMessageData + offSetStart + offset, sizeof(Float2) * meshMessage->meshData->nrUV);
    offset += sizeof(Float2) * meshMessage->meshData->nrUV;

    memcpy(meshMessage->meshData->indexPositions, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->nrI);
    offset += sizeof(int) * meshMessage->meshData->nrI;
    memcpy(meshMessage->meshData->indexNormals, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->nrI);
    offset += sizeof(int) * meshMessage->meshData->nrI;
    memcpy(meshMessage->meshData->indexUVs, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->nrI);
    offset += sizeof(int) * meshMessage->meshData->nrI;

    memcpy(meshMessage->meshData->trianglesPerFace, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->triangleCount);
}
void MayaLoader::ReadLight(int i)
{
    if(i != 4 && i != 3)
    {
        if(headerDidFit == true)
        { // headern ligger som vanligt, alltså där man är, headern får plats
            lightMessage = (LightMessage*)malloc(lightMessage_MaxSize);
            if(messageHeader.msgConfig == 1)
            { // meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
                memcpy(lightMessage, (unsigned char*)mMessageData, messageHeader.byteSize); // läser i början på filen utan nån offset
                localTail = messageHeader.byteSize + messageHeader.bytePadding;
            }
            else
            { // meddelandet får plats!!
                memcpy(lightMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal + localTail;
            }
        }
        else
        { // headern ligger på andra sidan, headern får inte plats
            // messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen
            // att hålla meddelandet

            if((localTail + messageHeader.byteSize) > mSize)
            { // meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
                cout << "Filemap too small, a message might be to big for the entire filemap";
            }
            else
            { // annars bara läs den rakt av från där denna redan är placerad
                lightMessage = (LightMessage*)malloc(lightMessage_MaxSize);
                memcpy(lightMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal;
            }
        }
    }
    else
    {
        ReadName();
    }


    if(i == 1)
    {
        LightAdded(messageHeader, lightMessage);
    }
    else if(i == 2)
    {
        LightChange(messageHeader, lightMessage); // tar hand om den aktualla meshen
    }
    else if(i == 4)
    {
        LightRenamed(messageHeader, nameMessage);
    }
    UpdateLightCBufferArray(); // updaterar constant buffern för ljus
    // flytta tailen
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    cout << "Move tail!!!!!" << localTail << "\n";
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    fileMapInfo.tail_ByteOffset = localTail;
    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}
void MayaLoader::ReadCamera(int i)
{
    if(i != 4 && i != 3)
    {
        if(headerDidFit == true)
        { // headern ligger som vanligt, alltså där man är, headern får plats
            cameraMessage = (CameraMessage*)malloc(cameraMessage_MaxSize);
            if(messageHeader.msgConfig == 1)
            { // meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
                memcpy(cameraMessage, (unsigned char*)mMessageData, messageHeader.byteSize); // läser i början på filen utan nån offset
                localTail = messageHeader.byteSize + messageHeader.bytePadding;
            }
            else
            { // meddelandet får plats!!
                memcpy(cameraMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal + localTail;
            }
        }
        else
        { // headern ligger på andra sidan, headern får inte plats
            // messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen
            // att hålla meddelandet

            if((localTail + messageHeader.byteSize) > mSize)
            { // meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
                cout << "Filemap too small, a message might be to big for the entire filemap";
            }
            else
            { // annars bara läs den rakt av från där denna redan är placerad
                cameraMessage = (CameraMessage*)malloc(cameraMessage_MaxSize);
                memcpy(cameraMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
                localTail = messageHeader.byteTotal;
            }
        }
    }
    else
    {
        ReadName();
    }

    if(i == 1)
    {
        CameraAdded(messageHeader, cameraMessage);
    }
    else if(i == 2)
    {
        CameraChange(messageHeader, cameraMessage); // tar hand om den aktualla meshen
    }
    else if(i == 4)
    {
        CameraRenamed(messageHeader, nameMessage);
    }
    else if(i == 5)
    {
        CameraSwitch(messageHeader, cameraMessage);
    }

    // flytta tailen
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    cout << "Move tail!!!!!" << localTail << "\n";
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    fileMapInfo.tail_ByteOffset = localTail;
    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}
void MayaLoader::ReadName()
{
    if(headerDidFit == true)
    { // headern ligger som vanligt, alltså där man är, headern får plats
        nameMessage = (NameMessage*)malloc(nameMessage_MaxSize);
        if(messageHeader.msgConfig == 1)
        { // meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
            memcpy(nameMessage, (unsigned char*)mMessageData, messageHeader.byteSize); // läser i början på filen utan nån offset
            localTail = messageHeader.byteSize + messageHeader.bytePadding;
        }
        else
        { // meddelandet får plats!!
            memcpy(nameMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
            localTail = messageHeader.byteTotal + localTail;
        }
    }
    else
    { // headern ligger på andra sidan, headern får inte plats
        // messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att
        // hålla meddelandet

        if((localTail + messageHeader.byteSize) > mSize)
        { // meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
            cout << "Filemap too small, a message might be to big for the entire filemap";
        }
        else
        { // annars bara läs den rakt av från där denna redan är placerad
            nameMessage = (NameMessage*)malloc(nameMessage_MaxSize);
            memcpy(nameMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
            localTail = messageHeader.byteTotal;
        }
    }
}

void MayaLoader::InitDX()
{
    // TODOKO Should not be here!! or should it? For standard shaders? Maybee in shadermanager
    // TODOLH Maybe shouldnt be here either. Moved it from shadermodulemanagerImplementation cos this guy needs to be able to switch shader
    // before drawing
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_vertexShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("BasicVertexShader.hlsl",
        ied, ARRAYSIZE(ied));
    m_pixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("BasicPixelShader.hlsl");

    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(rastDesc));
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.SlopeScaledDepthBias = 0.0f;
    rastDesc.DepthClipEnable = false;
    rastDesc.ScissorEnable = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.AntialiasedLineEnable = false;
    m_rasterizerState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateRasterizerState(rastDesc);
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    m_depthStencilState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateDepthStencilState(depthStencilDesc);
}

void MayaLoader::TransformAdded(MessageHeader mh, TransformMessage* mm)
{
    Transform* transform = new Transform(); // hitta den

    transform->transformDataP = mm; // referens för att den skall kunna tömma datan här, den är mallocad så
    transform->name = mm->objectName;
    transform->parentName = mm->parentName;
    transform->transformData = mm->transformData;

    if(transform->parentName[0] != 0)
    { // namnet är inte tomt -> den har en parent, så hitta den
        for(int i = 0; i < allTransforms.size(); i++)
        {
            if(strcmp(transform->parentName, allTransforms[i]->name) == 0)
            {
                transform->parent = allTransforms[i];
                break;
            }
        }
    }

    allTransforms.push_back(transform);

    // test!!!!
    // fileHandler = new FileHandler();
    // const char* skitName = "Hej" + 0;
    // fileHandler->SaveScene(MAX_NAME_SIZE, (char*)skitName,
    //	materials,
    //	allTransforms,
    //	allMeshTransforms,
    //	allLightTransforms);
    // test!!!!
}
void MayaLoader::TransformChange(MessageHeader mh, TransformMessage* mm)
{
    char* transformName = mm->objectName;
    Transform* transform = nullptr;

    for(int i = 0; i < allTransforms.size(); i++)
    {
        if(strcmp(transformName, allTransforms[i]->name) == 0)
        {
            transform = allTransforms[i];
            break;
        }
    }

    if(transform != nullptr)
    {
        transform->EmptyVariables();
        transform->transformDataP = mm;

        transform->name = transformName;
        transform->parentName = mm->parentName;
        transform->transformData = mm->transformData;

        if(transform->light != nullptr)
        {
            transform->light->UpdateCBuffer();
        }

        if(transform->parentName[0] != 0)
        { // namnet är inte tomt -> den har en parent, så hitta den
            for(int i = 0; i < allTransforms.size(); i++)
            {
                if(strcmp(transform->parentName, allTransforms[i]->name) == 0)
                {
                    transform->parent = allTransforms[i];
                    break;
                }
            }
        }
    }
}
void MayaLoader::TransformDeleted(MessageHeader mh,
                                  NameMessage* mm) // ta bort från alla vektorer!!! inte bara allTransforms!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
    char* objName = mm->name1;
    int tempIndex;

    for(int i = 0; i < allTransforms.size(); i++)
    {
        if(strcmp(objName, allTransforms[i]->name) == 0)
        {
            tempIndex = i;
            allTransforms.erase(allTransforms.begin() + i); // rätt index?
            break;
        }
    }
    for(int i = 0; i < allMeshTransforms.size(); i++) // ta oxå bort den från alla vektorer
    {
        if(strcmp(objName, allMeshTransforms[i]->name) == 0)
        {
            allMeshTransforms.erase(allMeshTransforms.begin() + i);
            return;
        }
    }
    for(int i = 0; i < allLightTransforms.size(); i++)
    {
        if(strcmp(objName, allLightTransforms[i]->name) == 0)
        {
            allLightTransforms.erase(allLightTransforms.begin() + i);
            return;
        }
    }
    for(int i = 0; i < allCameraTransforms.size(); i++)
    {
        if(strcmp(objName, allCameraTransforms[i]->name) == 0)
        {
            allCameraTransforms.erase(allCameraTransforms.begin() + i);
            return;
        }
    }
    delete allTransforms[tempIndex];
    free(mm); // kan freea denna direkt coz transformen ändå tas bort direkt, inget behöver sparas
}
void MayaLoader::TransformRenamed(MessageHeader mh, NameMessage* mm)
{
    char* oldName = mm->name2;
    Transform* transform = nullptr;

    for(int i = 0; i < allTransforms.size(); i++)
    {
        if(strcmp(oldName, allTransforms[i]->name) == 0)
        {
            transform = allTransforms[i];
            break;
        }
    }

    if(transform != nullptr)
    {
        transform->NameMessageChange(mm);
        transform->name = mm->name1;
    }
}

void MayaLoader::MeshAdded(MessageHeader mh, MeshMessage* mm)
{ // material måste alltid komma före meshes!!
    char* meshName = mm->objectName;
    Transform* meshTransform = nullptr; // hitta den
    Mesh* activeMesh = nullptr;
    int firstIndex = 0;

    for(int y = 0; y < mm->nrOfTransforms; y++) // loopa igenom alla transforms som denna meshen finns på
    {
        char* transformName = mm->transformNames[y].name;
        for(int i = 0; i < allTransforms.size(); i++)
        { // loopa igenom ALLA transforms och hitta alla transforms som meshen ska användas på
            if(strcmp(transformName, allTransforms[i]->name) == 0)
            {
                meshTransform = allTransforms[i];
                if(y == 0) // första transformen i meshlistan
                {
                    firstIndex = i; // får den första transformen som finns med i meshens transforms
                    meshTransform->mesh = new Mesh(); // skapa meshen
                    activeMesh = meshTransform->mesh;
                    activeMesh->numberOfTransforms = mm->nrOfTransforms;
                }
                else
                {
                    meshTransform->mesh = allTransforms[firstIndex]->mesh; // så att alla transforms för meshen får just denna mesh referencen
                }
                // activeMesh->transformNames.push_back(meshTransform->name); //mm->transformNames[i].name;
                allMeshTransforms.push_back(
                    meshTransform); // skickar in denna transform i allMeshTransforms oxå!! så den kommer vara refererad i båda vektorerna
                break;
            }
        }
    }
    if(meshTransform == nullptr)
    {
        printf("Hittade inte transformen");
    }
    else
    {

        for(int y = 0; y < mm->nrOfTransforms; y++) // loopa igenom alla transforms som denna meshen finns på
        {
            char* transformName = mm->transformNames[y].name;
            activeMesh->transformNames.push_back(transformName); // så att den har alla namnen hela tiden, oavsett om den hittat transformsen eller ej
        }

        activeMesh->meshDataP = mm; // endast för att kunna ta bort gamla värden properly
        activeMesh->name = mm->objectName;
        activeMesh->numberOfTransforms = mm->nrOfTransforms;
        activeMesh->materialName = mm->materialName;
        activeMesh->meshID = mm->meshID; // för instancing
        activeMesh->meshData = mm->meshData;
        activeMesh->transformNamesStruct = mm->transformNames;

        activeMesh->material = materials[0]; // default material
        if(activeMesh->material->name != nullptr)
        {
            for(int i = 0; i < materials.size(); i++)
            {
                if(strcmp(mm->materialName, materials[i]->name) == 0)
                {
                    activeMesh->material = materials[i];
                    break;
                }
            }
        }

        activeMesh->CreateBuffers();
    }
}
void MayaLoader::MeshChange(MessageHeader mh, MeshMessage* mm)
{ // MÅSTE HA TRANSFORMEN FÖRST, SEN SKAPA ETT MESH OBJEKT I TRANSFORMEN
    char* meshName = mm->objectName;
    //	char* transformName = mm->transformName;
    Transform* meshTransform = nullptr; // hitta den
    Mesh* activeMesh = nullptr;

    // int firstIndex = 0;
    for(int y = 0; y < mm->nrOfTransforms; y++) // hitta meshen
    {
        char* transformName = mm->transformNames[y].name;
        for(int i = 0; i < allMeshTransforms.size(); i++)
        {
            if(strcmp(transformName, allMeshTransforms[i]->name) == 0)
            {
                activeMesh = allMeshTransforms[i]->mesh;
                break;
            }
        }
        if(activeMesh != nullptr)
        {
            break;
        }
    }

    for(int y = 0; y < mm->nrOfTransforms; y++) // loopa igenom alla transforms som denna meshen finns på
    {
        char* transformName = mm->transformNames[y].name;
        for(int i = 0; i < allTransforms.size(); i++)
        { // loopa igenom ALLA transforms och hitta alla transforms som meshen ska användas på
            if(strcmp(transformName, allTransforms[i]->name) == 0)
            {
                meshTransform = allTransforms[i];
                meshTransform->mesh = activeMesh; // så att alla transforms för meshen får just denna mesh referencen

                activeMesh->transformNames.push_back(meshTransform->name); // mm->transformNames[i].name;

                // kolla ifall den redan finns i allMeshTransforms, lägg annars till den
                int check = 0;
                for(int m = 0; m < allMeshTransforms.size(); m++)
                {
                    if(strcmp(transformName, allMeshTransforms[m]->name) == 0)
                    {
                        check = 1;
                        break;
                    }
                }
                if(check == 0) // den fanns inte sen tidigare, lägg till den!
                {
                    allMeshTransforms.push_back(
                        meshTransform); // skickar in denna transform i allMeshTransforms oxå!! så den kommer vara refererad i båda vektorerna
                }
                break;
            }
        }
    }
    if(meshTransform == nullptr)
    {
        printf("Hittade inte transformen");
    }
    else
    {
        activeMesh = meshTransform->mesh;
        int oldNrVerts = activeMesh->meshData->nrI; // så att man vet ifall man kan remappa eller behöver bygga om vertexbuffern

        activeMesh->EmptyVariables(); // viktigt att göra dessa, annars kommer variablerna gå lost utan referens!!
        activeMesh->meshDataP = mm; // endast för att kunna ta bort gamla värden properly

        activeMesh->name = mm->objectName;
        activeMesh->materialName = mm->materialName;
        activeMesh->meshID = mm->meshID; // för instancing
        activeMesh->meshData = mm->meshData;
        activeMesh->transformNamesStruct = mm->transformNames;

        // activeMesh->material = materials[0]; //default material, den har redan ett material här
        for(int i = 0; i < materials.size(); i++)
        {
            if(strcmp(mm->materialName, materials[i]->name) == 0)
            {
                activeMesh->material = materials[i];
                break;
            }
        }

        if(activeMesh->meshData->nrI == oldNrVerts)
        {
            activeMesh->RemapVertexBuffer();
        }
        else
        {
            activeMesh->EmptyBuffers();
            activeMesh->CreateBuffers();
        }
    }
}
void MayaLoader::MeshRenamed(MessageHeader mh, NameMessage* mm)
{
    char* oldName = mm->name2;
    Mesh* mesh = nullptr;

    for(int i = 0; i < allMeshTransforms.size(); i++)
    {
        if(strcmp(oldName, allMeshTransforms[i]->mesh->name) == 0)
        {
            mesh = allMeshTransforms[i]->mesh;
            break;
        }
    }

    if(mesh != nullptr)
    {
        mesh->NameMessageChange(mm);
        mesh->name = mm->name1;
    }
}

void MayaLoader::MaterialAdded(MessageHeader mh, MaterialMessage* mm)
{
    // char* materialName = mm->objectName;

    Material* tempMat; // pekar på den mesh som redan finns storad eller så blir det en helt ny
    tempMat = new Material();
    tempMat->materialDataP = mm;

    tempMat->CreateCBuffer();
    tempMat->name = mm->objectName;
    tempMat->materialData = mm->materialData;

    // ladda in alla texturer
    tempMat->textureName = mm->textureName;
    tempMat->glowMapName = mm->glowMapName;
    tempMat->bumpMapName = mm->bumpMapName;
    tempMat->specularMapName = mm->specularMapName;

    tempMat->CreateTexture(tempMat->textureName);
    tempMat->CreateTexture(tempMat->glowMapName);
    /* tempMat->CreateTexture(tempMat->bumpMapName, tempMat->bumpTexture, tempMat->bumpTextureView);
     tempMat->CreateTexture(tempMat->specularMapName, tempMat->specularTexture, tempMat->specularTextureView);*/

    tempMat->UpdateCBuffer(); // lägger in de nya värdena i cbuffern
    materials.push_back(tempMat);
}
void MayaLoader::MaterialChange(MessageHeader mh, MaterialMessage* mm)
{
    char* materialName = mm->objectName;

    Material* tempMat = nullptr;

    for(int i = 0; i < materials.size(); i++)
    {
        if(strcmp(materialName, materials[i]->name) == 0)
        {
            tempMat = materials[i];
            break;
        }
    }
    tempMat->EmptyVariables(); // viktigt så att vi inte tappar referens till något som vi ska ta bort
    tempMat->materialDataP = mm;

    tempMat->name = mm->objectName;
    tempMat->materialData = mm->materialData;

    // ladda in alla texturer
    tempMat->textureName = mm->textureName;
    tempMat->glowMapName = mm->glowMapName;
    tempMat->bumpMapName = mm->bumpMapName;
    tempMat->specularMapName = mm->specularMapName;

    tempMat->CreateTexture(tempMat->textureName);
    tempMat->CreateTexture(tempMat->glowMapName);
    /* tempMat->CreateTexture(tempMat->bumpMapName, tempMat->bumpTexture, tempMat->bumpTextureView);
     tempMat->CreateTexture(tempMat->specularMapName, tempMat->specularTexture, tempMat->specularTextureView);*/

    tempMat->UpdateCBuffer();
}
void MayaLoader::MaterialDeleted(MessageHeader mh, NameMessage* mm)
{
    char* objName = mm->name1;
    int tempIndex;

    for(int i = 0; i < materials.size(); i++)
    {
        if(strcmp(objName, materials[i]->name) == 0)
        {
            tempIndex = i;
            materials.erase(materials.begin() + i); // rätt index?
            break;
        }
    }
    delete materials[tempIndex];
    free(mm);
}
void MayaLoader::MaterialRenamed(MessageHeader mh, NameMessage* mm)
{
    char* oldName = mm->name2;
    Material* material = nullptr;

    for(int i = 0; i < materials.size(); i++)
    {
        if(strcmp(oldName, materials[i]->name) == 0)
        {
            material = materials[i];
            break;
        }
    }

    if(material != nullptr)
    {
        material->NameMessageChange(mm);
        material->name = mm->name1;
    }
}

void MayaLoader::LightAdded(MessageHeader mh, LightMessage* mm)
{
    char* transformName = mm->transformName;
    Transform* lightTransform = nullptr; // hitta den
    Light* tempLight = nullptr;

    for(int i = 0; i < allTransforms.size(); i++)
    {
        if(strcmp(transformName, allTransforms[i]->name) == 0)
        {
            lightTransform = allTransforms[i];
            break;
        }
    }
    if(lightTransform == nullptr)
    {
        printf("Hittade inte transformen");
    }
    else
    {
        lightTransform->light = new Light();
        tempLight = lightTransform->light;
        tempLight->lightDataP = mm; // för att kunna deallokera messaget (mm) det är mallocat

        tempLight->transform = lightTransform;

        tempLight->name = mm->objectName;
        tempLight->lightData = mm->lightdata;
        tempLight->UpdateCBuffer();

        allLightTransforms.push_back(lightTransform); // lägger den i lightTransformsen
    }
}
void MayaLoader::LightChange(MessageHeader mh, LightMessage* mm)
{

    char* transformName = mm->transformName;
    Transform* lightTransform = nullptr; // hitta den
    Light* tempLight = nullptr;

    for(int i = 0; i < allLightTransforms.size(); i++)
    {
        if(strcmp(transformName, allLightTransforms[i]->name) == 0)
        {
            lightTransform = allLightTransforms[i];
            break;
        }
    }
    if(lightTransform == nullptr)
    {
        printf("Hitta inte transformen");
    }
    else
    {
        tempLight = lightTransform->light;

        tempLight->EmptyVariables();
        tempLight->lightDataP = mm; // för att kunna deallokera messaget (mm) det är mallocat

        tempLight->name = mm->objectName;
        tempLight->lightData = mm->lightdata;
        tempLight->UpdateCBuffer();
    }
}
void MayaLoader::LightRenamed(MessageHeader mh, NameMessage* mm)
{
    char* oldName = mm->name2;
    Light* light = nullptr;

    for(int i = 0; i < allLightTransforms.size(); i++)
    {
        if(strcmp(oldName, allLightTransforms[i]->light->name) == 0)
        {
            light = allLightTransforms[i]->light;
            break;
        }
    }

    if(light != nullptr)
    {
        light->NameMessageChange(mm);
        light->name = mm->name1;
    }
}

void MayaLoader::CameraAdded(MessageHeader mh, CameraMessage* mm)
{
    char* transformName = mm->transformName;
    Transform* cameraTransform = nullptr; // hitta den
    CameraObj* tempCamera = nullptr;

    for(int i = 0; i < allTransforms.size(); i++)
    {
        if(strcmp(transformName, allTransforms[i]->name) == 0)
        {
            cameraTransform = allTransforms[i];
            break;
        }
    }
    if(cameraTransform == nullptr)
    {
        printf("Hittade inte transformen");
    }
    else
    {
        cameraTransform->camera = new CameraObj();
        tempCamera = cameraTransform->camera;

        tempCamera->cameraDataP = mm;

        tempCamera->CreateCBuffer();

        tempCamera->name = mm->objectName;
        tempCamera->transform = cameraTransform;
        tempCamera->cameraData = mm->cameraData;
        tempCamera->UpdateCBuffer(screenWidth, screenHeight);

        currentCameraTransform = cameraTransform; // denna är temporär, ska bara ske vid CameraSwitch funktionen

        allCameraTransforms.push_back(cameraTransform);
    }
}
void MayaLoader::CameraChange(MessageHeader mh, CameraMessage* mm)
{
    char* transformName = mm->transformName;
    Transform* cameraTransform = nullptr; // hitta den för att hitta vilken camera den syftar på
    CameraObj* tempCamera = nullptr;

    for(int i = 0; i < allCameraTransforms.size(); i++)
    {
        if(strcmp(transformName, allCameraTransforms[i]->name) == 0)
        {
            cameraTransform = allCameraTransforms[i];
            break;
        }
    }
    if(cameraTransform == nullptr)
    { // ha kvar pekaren på den gamla transformen!
        printf("Hittade inte transformen");
    }
    else
    {
        tempCamera = cameraTransform->camera;
        tempCamera->EmptyVariables();
        tempCamera->cameraDataP = mm;
        // tempCamera->transform = cameraTransform; //ge kameran reference till transformen, bara vid starten? den byter väl inte transform?

        tempCamera->name = mm->objectName;
        tempCamera = cameraTransform->camera;
        tempCamera->cameraData = mm->cameraData;
        tempCamera->UpdateCBuffer(screenWidth, screenHeight);
    }
}
void MayaLoader::CameraRenamed(MessageHeader mh, NameMessage* mm)
{
    char* oldName = mm->name2;
    CameraObj* camera = nullptr;

    for(int i = 0; i < allCameraTransforms.size(); i++)
    {
        if(strcmp(oldName, allCameraTransforms[i]->camera->name) == 0)
        {
            camera = allCameraTransforms[i]->camera;
            break;
        }
    }

    if(camera != nullptr)
    {
        camera->NameMessageChange(mm);
        camera->name = mm->name1;
    }
}
void MayaLoader::CameraSwitch(MessageHeader mh, CameraMessage* mm)
{
    char* newActiveCameraTransformName = mm->transformName;

    for(int i = 0; i < allCameraTransforms.size(); i++)
    {
        if(strcmp(newActiveCameraTransformName, allCameraTransforms[i]->name) == 0)
        {
            currentCameraTransform = allCameraTransforms[i];
            break;
        }
    }
}

bool MayaLoader::UpdateCameraValues()
{
    if(currentCameraTransform != nullptr)
    {
        // currentCameraTransform->UpdateCBuffer();
        currentCameraTransform->camera->UpdateCBuffer(screenWidth, screenHeight);

        // gDeviceContext->UpdateSubresource(cCameraConstantBuffer, 0, NULL, &cameraCBufferData, 0, 0);

        return true;
    }
    return false;
}

void MayaLoader::CreateLightCBufferArray()
{
}
void MayaLoader::UpdateLightCBufferArray()
{
    int nrLights = 0;
    for(int i = 0; i < allLightTransforms.size() && i < 128; i++)
    {
        lightCBufferDataArray.lightDatas[i] = allLightTransforms[i]->light->lightCBufferData;
        nrLights++;
    }
    lightCBufferDataArray.NumLights = nrLights;
    lightCBufferDataArray.pad[0] = {0};

}

void MayaLoader::SaveScene()
{
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo)); // h?mta filemapinfo datan om jag har f?tt mutexInfo till den

    const char* skitName = fileMapInfo.outFileName;
    fileHandler->SaveScene(MAX_NAME_SIZE, (char*)skitName, materials, allTransforms, allMeshTransforms, allLightTransforms);
}