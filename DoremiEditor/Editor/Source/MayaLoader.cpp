#include "MayaLoader.h"

Mutex mutexInfo("__info_Mutex__");
MayaLoader::MayaLoader(ID3D11Device* gd, ID3D11DeviceContext* gdc, UINT screenWidth, UINT screenHeight)
{
    this->gDevice = gd;
    this->gDeviceContext = gdc;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    CreateFileMaps(1024 * 1024 * 10);
    InitVariables();

    fileHandler = new FileHandler();
    Material* defaultMaterial = new Material(gDevice, gDeviceContext);
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
    D3D11_BUFFER_DESC cameraBufferDesc;
    memset(&cameraBufferDesc, 0, sizeof(cameraBufferDesc));
    cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cameraBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    cameraBufferDesc.ByteWidth = sizeof(CameraCBufferData);
    gDevice->CreateBuffer(&cameraBufferDesc, NULL, &cDefaultCameraConstantBuffer);

    XMStoreFloat4x4(&defaultCameraCBufferData.view, XMMatrixTranspose(XMMatrixIdentity()));
    XMStoreFloat4x4(&defaultCameraCBufferData.projection, XMMatrixTranspose(XMMatrixIdentity()));
    gDeviceContext->UpdateSubresource(cDefaultCameraConstantBuffer, 0, NULL, &defaultCameraCBufferData, 0, 0); // default buffer
    // fpsCam.SetLens(0.25f*3.14f, screenWidth / screenHeight, 1.0f, 1000.0f);

    CreateLightCBufferArray();

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    gDevice->CreateSamplerState(&samplerDesc, &wrap_Sampstate);

    D3D11_SAMPLER_DESC samplerDesc2;
    ZeroMemory(&samplerDesc2, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc2.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc2.MaxAnisotropy = 16;
    gDevice->CreateSamplerState(&samplerDesc2, &clamp_Sampstate);
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
    UINT32 vertexSize2 = sizeof(float) * 8;
    UINT32 offset2 = 0;
    // set rätt constantbuffers, ljus, kamera och material stuff!
    UpdateCameraValues(); // updaterar oxå camera cbuffern

    for(int i = 0; i < allMeshTransforms.size(); i++)
    {
        Mesh* currMesh = allMeshTransforms[i]->mesh;
        gDeviceContext->IASetVertexBuffers(0, 1, &currMesh->vertexBuffer, &vertexSize2, &offset2);
        gDeviceContext->IASetIndexBuffer(currMesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        if(currMesh->material != nullptr)
        {
            gDeviceContext->PSSetConstantBuffers(1, 1, &currMesh->material->materialCbuffer);

            if(currMesh->material->diffuseTextureView != nullptr)
            {
                gDeviceContext->PSSetShaderResources(0, 1, &currMesh->material->diffuseTextureView);
            }

            gDeviceContext->PSSetSamplers(0, 1, &wrap_Sampstate);
        }
        else
        {
            gDeviceContext->PSSetConstantBuffers(1, 1, &materials[0]->materialCbuffer);
        }
        // transformdata ligger på plats 0, material på 1, osv
        // set transformcbufferns värden, updatesubresource
        allMeshTransforms[i]->UpdateCBuffer(); // slå först ihop med parentens värden innan vi updaterar cbuffern
        UpdateLightCBufferArray();
        gDeviceContext->Draw(currMesh->nrIndecies, 0);

        // gDeviceContext->DrawIndexed(currMesh->nrIndecies, 0, 0);
    }
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

void MayaLoader::TransformAdded(MessageHeader mh, TransformMessage* mm)
{
    Transform* transform = new Transform(gDevice, gDeviceContext); // hitta den

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
                    meshTransform->mesh = new Mesh(gDevice, gDeviceContext); // skapa meshen
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
    tempMat = new Material(gDevice, gDeviceContext);
    tempMat->materialDataP = mm;

    tempMat->CreateCBuffer();
    tempMat->name = mm->objectName;
    tempMat->materialData = mm->materialData;

    // ladda in alla texturer
    tempMat->textureName = mm->textureName;
    tempMat->glowMapName = mm->glowMapName;
    tempMat->bumpMapName = mm->bumpMapName;
    tempMat->specularMapName = mm->specularMapName;

    tempMat->CreateTexture(tempMat->textureName, tempMat->diffuseTexture, tempMat->diffuseTextureView);
    tempMat->CreateTexture(tempMat->glowMapName, tempMat->glowTexture, tempMat->glowTextureView);
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

    tempMat->CreateTexture(tempMat->textureName, tempMat->diffuseTexture, tempMat->diffuseTextureView);
    tempMat->CreateTexture(tempMat->glowMapName, tempMat->glowTexture, tempMat->glowTextureView);
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
        lightTransform->light = new Light(gDevice, gDeviceContext);
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
        cameraTransform->camera = new CameraObj(gDevice, gDeviceContext);
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
        gDeviceContext->VSSetConstantBuffers(10, 1, &currentCameraTransform->camera->cameraCbuffer);

        return true;
    }
    gDeviceContext->VSSetConstantBuffers(10, 1, &cDefaultCameraConstantBuffer); // kör på default identitesmatriser annars
    return false;
}

void MayaLoader::CreateLightCBufferArray()
{
    D3D11_BUFFER_DESC cbDesc = {0};
    cbDesc.ByteWidth = sizeof(LightCBufferDataArray); /// ska nog inte vara arrayen utan varje enskillt element, använd sedan shaderresourceview och
    /// store:a flera av denna buffern
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    // Create the buffer.
    gDevice->CreateBuffer(&cbDesc, NULL, &lightCbufferArray);
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

    gDeviceContext->UpdateSubresource(lightCbufferArray, 0, NULL, &lightCBufferDataArray, 0, 0);
    gDeviceContext->PSSetConstantBuffers(2, 1, &lightCbufferArray); // kör på default identitesmatriser annars?
}

void MayaLoader::SaveScene()
{
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo)); // h?mta filemapinfo datan om jag har f?tt mutexInfo till den

    const char* skitName = fileMapInfo.outFileName;
    fileHandler->SaveScene(MAX_NAME_SIZE, (char*)skitName, materials, allTransforms, allMeshTransforms, allLightTransforms);
}