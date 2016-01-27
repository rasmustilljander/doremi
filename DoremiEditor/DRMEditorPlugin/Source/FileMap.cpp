#include <FileMap.hpp>
#include <DoremiEditor/Core/Include/MessageType.hpp>


FileMapping::FileMapping()
{
    // mutexInfo.Unlock();
    //_headerSize = sizeof(MessageHeader);
    headerFit;
}

FileMapping::~FileMapping()
{
    // mutexInfo.Unlock();

    std::string str;
    bool m = UnmapViewOfFile((LPCVOID)mMessageData);
    /*str = GetLastErrorAsString();
    FileMapping::printInfo("Last Error: " + MString(str.c_str()));*/
    if(GetLastError() != 0)
    {
        FileMapping::printInfo("Error");
    }
    if(m != 0) FileMapping::printInfo("Filemap closed");
    m = CloseHandle(hMessageFileMap);
    /*str = GetLastErrorAsString();
    FileMapping::printInfo("Last Error: " + MString(str.c_str()));*/
    if(GetLastError() != 0)
    {
        FileMapping::printInfo("Error");
    }
    if(m != 0) FileMapping::printInfo("Filemap closed");


    bool n = UnmapViewOfFile((LPCVOID)mInfoData);
    /*str = GetLastErrorAsString();
    FileMapping::printInfo("Last Error: " + MString(str.c_str()));*/
    if(GetLastError() != 0)
    {
        FileMapping::printInfo("Error");
    }
    if(n != 0) FileMapping::printInfo("InfoFilemap closed");
    n = CloseHandle(hInfoFileMap);
    /*str = GetLastErrorAsString();
    FileMapping::printInfo("Last Error: " + MString(str.c_str()));*/
    if(GetLastError() != 0)
    {
        FileMapping::printInfo("Error");
    }
    if(n != 0) FileMapping::printInfo("InfoFilemap closed");
}

void FileMapping::CreateFileMaps(bool debug)
{
    this->debug = debug;
    FileMapping::printInfo("\n***** LOADING FFILEMAPS *****");
    mutexInfo.Create("__info_Mutex__");
    mInfoSize = 256;
    // info filemapen
    SECURITY_ATTRIBUTES security;
    security.lpSecurityDescriptor = nullptr;
    hInfoFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, (DWORD)0, mInfoSize, (LPCWSTR) "Global\\infoFileMap");
    // hInfoFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPCWSTR)"infoFileMap");

    mInfoData = MapViewOfFile(hInfoFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);


    if(hInfoFileMap == NULL)
    {
        FileMapping::printInfo("Couldn't create infofilemap");
    }

    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        FileMapping::printInfo("Infofilemap exists, you get a handle to it!");
        GetFilemapInfoValues();
    }
    else
    { // first, sätter de första värdena på filemapinfon
        FileMapping::printInfo("Creating new infofilemap, JAG SKA INTE VARA FÖRST :'(");
        MGlobal::displayError(GetLastErrorAsString().c_str());
        SetFilemapInfoValues(0, 0, 256, 1024 * 1024);
    }


    FileMapping::printInfo("MSIZE " + MString() + mSize);
    memoryPadding = fileMapInfo.non_accessmemoryOffset;

    // mSize = 2048; //denna ska hämtas
    hMessageFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, (DWORD)0, mSize, (LPCWSTR) "Global\\messageFileMap");

    mMessageData = MapViewOfFile(hMessageFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if(hMessageFileMap == NULL)
    {
        FileMapping::printInfo("Couldn't create filemap");
    }
    else if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        FileMapping::printInfo("Filemap exists, you get a handle to it!");
    }
    else
    {

        FileMapping::printInfo("Creating new filemap");
    }


    FileMapping::printInfo("FileMapSize: " + MString() + mSize + " " + MString() + fileMapInfo.non_accessmemoryOffset);
    FileMapping::printInfo("***** FILEMAP LOADED *****");
}

void FileMapping::GetFilemapInfoValues()
{
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo)); // får inget värde!!!!!!! default värdet är fortfarande kvar
    mSize = fileMapInfo.messageFilemap_Size;
    FileMapping::printInfo("MessageFMMemorySize: " + MString() + fileMapInfo.messageFilemap_Size);
    FileMapping::printInfo("Nonaccess: " + MString() + fileMapInfo.non_accessmemoryOffset);
    mutexInfo.Unlock();
}

void FileMapping::SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize)
{
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    if(headPlacement >= 0) fileMapInfo.head_ByteOffset = headPlacement;
    if(tailPlacement >= 0) fileMapInfo.tail_ByteOffset = tailPlacement;
    if(nonAccessMemoryPlacement >= 0) fileMapInfo.non_accessmemoryOffset = nonAccessMemoryPlacement;
    if(messageFileMapTotalSize > 0) fileMapInfo.messageFilemap_Size = messageFileMapTotalSize;
    mSize = messageFileMapTotalSize;
    memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
    mutexInfo.Unlock();
}
bool FileMapping::tryWriteTransform(MessageInfo& msg, TransformInfo& tinfo)
{
    bool returnBool = false;
    if(msg.msgType == MessageType::msgDeleted)
    {
    }
    else if(msg.msgType == MessageType::msgRenamed)
    {
    }
    else
    {
        MessageHeader mHeader = createHeaderTransform(msg, tinfo);
        int cfg = findWriteConfig(mHeader);
        if(cfg != 0)
        {
            if(writeTransform(mHeader, createMessageTransform(msg, tinfo), cfg) == true)
            {
                returnBool = true;
            }
        }
        else
        {
            returnBool = false;
        }
    }

    return returnBool;
}
bool FileMapping::tryWriteMesh(MessageInfo& msg, MeshInfo& minfo)
{
    MessageHeader mHeader = createHeaderMesh(msg, minfo);
    int cfg = findWriteConfig(mHeader);
    if(cfg != 0)
    {
        if(writeMesh(mHeader, createMessageMesh(msg, minfo), cfg) == true)
        {
            return true;
        }
    }
    else
    {
        return false;
    }
    return false;

    /*FileMapping::printInfo("FileMap Msg: Mesh Message found");
    MessageHeader mHeader = createHeaderMesh(msg, minfo);
    int cfg = findWriteConfig(mHeader);
    createMessageMesh(msg, minfo);*/

    return false;
}
bool FileMapping::tryWriteCamera(MessageInfo& msg, CameraInfo& cinfo)
{
    MessageHeader mHeader = createHeaderCamera(msg, cinfo);
    int cfg = findWriteConfig(mHeader);
    if(cfg != 0)
    {
        if(writeCamera(mHeader, createMessageCamera(msg, cinfo), cfg) == true)
        {
            return true;
        }
    }
    else
    {
        return false;
    }
    return false;
}
bool FileMapping::tryWriteMaterial(MessageInfo& msg, MaterialInfo& minfo)
{

    MessageHeader mHeader = createHeaderMaterial(msg, minfo);
    int cfg = findWriteConfig(mHeader);
    if(cfg != 0)
    {
        if(writeMaterial(mHeader, createMessageMaterial(msg, minfo), cfg) == true)
        {
            return true;
        }
    }
    else
    {
        return false;
    }


    return false;
}
bool FileMapping::tryWriteLight(MessageInfo& msg, LightInfo& linfo)
{
    MessageHeader mHeader = createHeaderLight(msg, linfo);
    int cfg = findWriteConfig(mHeader);
    if(cfg != 0)
    {
        if(writeLight(mHeader, createMessageLight(msg, linfo), cfg) == true)
        {
            return true;
        }
    }
    else
    {
        return false;
    }
    return false;
}
bool FileMapping::tryWriteRenameDelete(MessageInfo& info, RenameDeleteInfo& msg)
{

    MessageHeader mHeader = createHeaderRenameDelete(info);
    int cfg = findWriteConfig(mHeader);
    if(cfg != 0)
    {
        if(writeNodeRenamedDelete(mHeader, createMessageRenameDelete(info, msg), cfg) == true)
        {
            return true;
        }
    }
    else
    {
        return false;
    }
    return false;
}

void FileMapping::printInfo(MString instr) { MGlobal::displayInfo("[DRM] " + instr); }
void FileMapping::printError(MString instr) { MGlobal::displayError("[DRM] " + instr); }
void FileMapping::printWarning(MString instr) { MGlobal::displayWarning("[DRM] " + instr); }

// Write config return values
// 0: Can't write
// 1: Can write normally
// 2: Header fits before buffer end, but message will have to be moved to the beginning of the buffer
int FileMapping::findWriteConfig(MessageHeader& hdr)
{
    while(mutexInfo.Lock(1000) == false)
        Sleep(10);
    memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
    localHead = fileMapInfo.head_ByteOffset;
    localTail = fileMapInfo.tail_ByteOffset;
    mutexInfo.Unlock();

    if(localHead >= localTail)
    {
        if(hdr.byteSize + hdr.bytePadding + localHead + memoryPadding <= mSize)
        {
            hdr.msgConfig = 0;
            FileMapping::printInfo("***   MSG Config (CAN WRITE NORMALLY)");
            return 1;
        }
        else if(localHead + sizeof(MessageHeader) <= mSize && makeMultiple(hdr.byteSize, 256) + memoryPadding <= localTail)
        {
            FileMapping::printInfo("***   MSG Config (CAN WRITE WITH SPLIT)");
            size_t tempTotal;
            hdr.msgConfig = 1;
            tempTotal = makeMultiple(hdr.byteSize, 256);
            hdr.bytePadding = tempTotal - hdr.byteSize;
            return 2;
        }
        else if(hdr.byteTotal + memoryPadding <= localTail)
        {
            hdr.msgConfig = 2;
            return 3;
        }
    }
    else
    {
        if(hdr.byteTotal + localHead + memoryPadding <= localTail)
        {
            hdr.msgConfig = 0;
            FileMapping::printInfo("***   MSG Config (CAN WRITE NORMALLY)");
            return 1;
        }
    }
    FileMapping::printInfo("***   MSG Config (CANNOT WRITE)");
    // FileMapping::printInfo("***   HEAD POSITION: " + MString() + localHead + " TAIL POSISH: " +MString() + localTail);

    return 0;
}
bool FileMapping::writeTransform(MessageHeader& hdr, TransformMessage& tdata, int config)
{
    int cfg = config;
    switch(cfg)
    {
        case 1:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));

            localHead += sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + localHead, &tdata, hdr.byteSize);

            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            if(localHead == mSize)
            {
                localHead = 0;
            }
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 2:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            localHead = 0;
            memcpy((unsigned char*)mMessageData, &tdata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 3:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData, &hdr, sizeof(MessageHeader));
            localHead = sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + sizeof(MessageHeader), &tdata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);

            return true;
            break;
    }
    return false;
}
bool FileMapping::writeMesh(MessageHeader& hdr, MeshMessage& mdata, int config)
{
    int cfg = config;
    switch(cfg)
    {
        case 1:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            size_t tempHead;
            // tempHead = 0;
            tempHead = sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata, sizeof(char) * 300);
            tempHead += sizeof(char) * 300;

            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshID, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.materialID, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.vertCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.normalCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.UVCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.indCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.triCount, sizeof(int));
            tempHead += sizeof(int);


            /*memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata, sizeof(int) * 2);
            tempHead += sizeof(int) * 2;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata+sizeof(int)*2, sizeof(char) * 300);
            tempHead += sizeof(char) * 300;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata+sizeof(int)*2+300, sizeof(int) * 5);
            tempHead += sizeof(int) * 5;*/
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.positions, sizeof(float) * 3 * mdata.meshData.vertCount);
            tempHead += sizeof(float) * 3 * mdata.meshData.vertCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.normals, sizeof(float) * 3 * mdata.meshData.normalCount);
            tempHead += sizeof(float) * 3 * mdata.meshData.normalCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.uvs, sizeof(float) * 2 * mdata.meshData.UVCount);
            tempHead += sizeof(float) * 2 * mdata.meshData.UVCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.indexPositions, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.indexNormals, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.indexUVs, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.trianglesPerFace, sizeof(int) * mdata.meshData.triCount);
            tempHead += sizeof(int) * mdata.meshData.triCount;

            FileMapping::printInfo("***** (CFG " + MString() + cfg + ") " + MString() + (tempHead + hdr.bytePadding) + " " + MString() + hdr.byteTotal);
            tempHead += hdr.bytePadding;

            localHead += tempHead;
            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            if(localHead == mSize)
            {
                localHead = 0;
            }
            fileMapInfo.head_ByteOffset += hdr.byteTotal;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            FileMapping::printInfo("* WOW EN VERTEX: " + MString() + mdata.meshData.vertCount + " " + MString() + mdata.materialID + " " + mdata.materialName);
            return true;
            break;

        case 2:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            localHead = 0;
            tempHead = sizeof(MessageHeader);


            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata, sizeof(char) * 300);
            tempHead += sizeof(char) * 300;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshID, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.materialID, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.vertCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.normalCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.UVCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.indCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.triCount, sizeof(int));
            tempHead += sizeof(int);

            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.positions, sizeof(float) * 3 * mdata.meshData.vertCount);
            tempHead += sizeof(float) * 3 * mdata.meshData.vertCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.normals, sizeof(float) * 3 * mdata.meshData.normalCount);
            tempHead += sizeof(float) * 3 * mdata.meshData.normalCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.uvs, sizeof(float) * 2 * mdata.meshData.UVCount);
            tempHead += sizeof(float) * 2 * mdata.meshData.UVCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.indexPositions, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.indexNormals, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.indexUVs, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, mdata.meshData.trianglesPerFace, sizeof(int) * mdata.meshData.triCount);
            tempHead += sizeof(int) * mdata.meshData.triCount;

            FileMapping::printInfo("***** (CFG " + MString() + cfg + ") " + MString() + (tempHead + hdr.bytePadding) + " " + MString() + hdr.byteTotal);
            tempHead += hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 3:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData, &hdr, sizeof(MessageHeader));
            tempHead = sizeof(MessageHeader);

            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata, sizeof(char) * 300);
            tempHead += sizeof(char) * 300;
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshID, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.materialID, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.vertCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.normalCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.UVCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.indCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + localHead + tempHead, &mdata.meshData.triCount, sizeof(int));
            tempHead += sizeof(int);
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.positions, sizeof(float) * 3 * mdata.meshData.vertCount);
            tempHead += sizeof(float) * 3 * mdata.meshData.vertCount;
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.normals, sizeof(float) * 3 * mdata.meshData.normalCount);
            tempHead += sizeof(float) * 3 * mdata.meshData.normalCount;
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.uvs, sizeof(float) * 2 * mdata.meshData.UVCount);
            tempHead += sizeof(float) * 2 * mdata.meshData.UVCount;
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.indexPositions, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.indexNormals, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.indexUVs, sizeof(int) * mdata.meshData.indCount);
            tempHead += sizeof(int) * mdata.meshData.indCount;
            memcpy((unsigned char*)mMessageData + tempHead, mdata.meshData.trianglesPerFace, sizeof(int) * mdata.meshData.triCount);
            tempHead += sizeof(int) * mdata.meshData.triCount;

            FileMapping::printInfo("***** (CFG " + MString() + cfg + ") " + MString() + (tempHead + hdr.bytePadding) + " " + MString() + hdr.byteTotal);
            tempHead += hdr.bytePadding;
            localHead += tempHead;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;
    }


    return false;
}
bool FileMapping::writeCamera(MessageHeader& hdr, CameraMessage& cdata, int config)
{
    int cfg = config;
    switch(cfg)
    {
        case 1:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));

            localHead += sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + localHead, &cdata, hdr.byteSize);

            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            if(localHead == mSize)
            {
                localHead = 0;
            }
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 2:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            localHead = 0;
            memcpy((unsigned char*)mMessageData, &cdata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 3:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData, &hdr, sizeof(MessageHeader));
            localHead = sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + sizeof(MessageHeader), &cdata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;
    }
    return false;
}
bool FileMapping::writeMaterial(MessageHeader& hdr, MaterialMessage& mdata, int config)
{
    int cfg = config;
    switch(cfg)
    {
        case 1:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));

            localHead += sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + localHead, &mdata, hdr.byteSize);

            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            if(localHead == mSize)
            {
                localHead = 0;
            }
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 2:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            localHead = 0;
            memcpy((unsigned char*)mMessageData, &mdata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 3:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData, &hdr, sizeof(MessageHeader));
            localHead = sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + sizeof(MessageHeader), &mdata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;

            break;
    }
    return false;
}
bool FileMapping::writeLight(MessageHeader& hdr, LightMessage& ldata, int config)
{
    int cfg = config;
    switch(cfg)
    {
        case 1:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));

            localHead += sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + localHead, &ldata, hdr.byteSize);

            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            if(localHead == mSize)
            {
                localHead = 0;
            }
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 2:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            localHead = 0;
            memcpy((unsigned char*)mMessageData, &ldata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 3:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData, &hdr, sizeof(MessageHeader));
            localHead = sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + sizeof(MessageHeader), &ldata, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;


            break;
    }
    return false;
}
bool FileMapping::writeNodeRenamedDelete(MessageHeader& hdr, RenameDeleteMessage& msg, int config)
{
    int cfg = config;
    switch(cfg)
    {
        case 1:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));

            localHead += sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + localHead, &msg, hdr.byteSize);

            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            if(localHead == mSize)
            {
                localHead = 0;
            }
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 2:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData + localHead, &hdr, sizeof(MessageHeader));
            localHead = 0;
            memcpy((unsigned char*)mMessageData, &msg, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;
            break;

        case 3:
            PrintFileMapInfo(false);
            memcpy((unsigned char*)mMessageData, &hdr, sizeof(MessageHeader));
            localHead = sizeof(MessageHeader);
            memcpy((unsigned char*)mMessageData + sizeof(MessageHeader), &msg, hdr.byteSize);
            localHead += hdr.byteSize + hdr.bytePadding;

            while(mutexInfo.Lock(1000) == false)
                Sleep(10);
            memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
            fileMapInfo.head_ByteOffset = localHead;
            memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
            mutexInfo.Unlock();
            PrintFileMapInfo(true);
            return true;


            break;
    }
    return false;
}


MessageHeader FileMapping::createHeaderMesh(MessageInfo& msginfo, MeshInfo& minfo)
{
    size_t totalSize;
    size_t msgSize;
    size_t padding;
    size_t infoSize;

    infoSize = 300 * sizeof(char);
    infoSize += sizeof(int) * 2;

    MeshInfo mInfo = minfo;
    msgSize = 5 * sizeof(int);
    msgSize += 3 * mInfo.meshData.vertCount * sizeof(float);
    msgSize += 3 * mInfo.meshData.normalCount * sizeof(float);
    msgSize += 2 * (mInfo.meshData.UVCount * sizeof(float));
    msgSize += 3 * (mInfo.meshData.indCount * sizeof(int));
    msgSize += mInfo.meshData.triCount * sizeof(int);

    if(debug)
    {
        FileMapping::printInfo(":: Info byte size: " + MString() + infoSize);
        FileMapping::printInfo(":: Mesh data byte size: " + MString() + msgSize);
        FileMapping::printInfo(":: Header byte size: " + MString() + sizeof(MessageHeader));

        // msgSize += _headerSize;
        FileMapping::printInfo(":: MESSAGE BYTE SIZE: " + MString() + msgSize);

        FileMapping::printInfo(":: Padding byte size: " + MString() + padding);
        FileMapping::printInfo(":: TOTAL BYTE SIZE: " + MString() + totalSize);
        FileMapping::printInfo(":: Node Name Length: " + MString(msginfo.nodeName.c_str()) + MString() + mInfo.nodeName.length());
    }
    msgSize += infoSize;
    totalSize = makeMultiple(msgSize + sizeof(MessageHeader), 256);
    padding = totalSize - msgSize;

    MessageHeader hdr;
    hdr.nodeType = msginfo.nodeType;
    hdr.messageType = msginfo.msgType;
    hdr.byteTotal = totalSize;
    hdr.byteSize = msgSize;
    hdr.bytePadding = padding;

    return hdr;
    // return 0;
}
MessageHeader FileMapping::createHeaderTransform(MessageInfo& msginfo, TransformInfo& tInfo)
{
    size_t totalSize;
    size_t msgSize;
    size_t padding;
    size_t infoSize;
    // infoSize = 200 * sizeof(char);
    msgSize = sizeof(TransformMessage) + sizeof(MessageHeader);

    totalSize = makeMultiple(msgSize, 256);
    padding = totalSize - msgSize;
    FileMapping::printInfo("*   Transform Message Sizes(HDR,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() +
                           (msgSize - sizeof(MessageHeader)) + " " + MString() + padding + " " + MString() + totalSize);

    MessageHeader hdr;
    hdr.nodeType = msginfo.nodeType;
    hdr.messageType = msginfo.msgType;
    hdr.byteTotal = totalSize;
    hdr.byteSize = msgSize - sizeof(MessageHeader);
    hdr.bytePadding = padding;

    return hdr;
    // return 0;
}
MessageHeader FileMapping::createHeaderCamera(MessageInfo& msginfo, CameraInfo& cInfo)
{
    size_t totalSize;
    size_t msgSize;
    size_t padding;
    size_t infoSize;
    // infoSize = 200 * sizeof(char);
    msgSize = sizeof(CameraMessage) + sizeof(MessageHeader);

    totalSize = makeMultiple(msgSize, 256);
    padding = totalSize - msgSize;
    FileMapping::printInfo("*   Camera Message Sizes(HDR,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() +
                           (msgSize - sizeof(MessageHeader)) + " " + MString() + padding + " " + MString() + totalSize);

    MessageHeader hdr;
    hdr.nodeType = msginfo.nodeType;
    hdr.messageType = msginfo.msgType;
    hdr.byteTotal = totalSize;
    hdr.byteSize = msgSize - sizeof(MessageHeader);
    hdr.bytePadding = padding;

    return hdr;
    // return 0;
}
MessageHeader FileMapping::createHeaderMaterial(MessageInfo& msginfo, MaterialInfo& mInfo)
{
    size_t totalSize;
    size_t msgSize;
    size_t padding;
    size_t infoSize;
    // infoSize = 200 * sizeof(char);
    msgSize = sizeof(MaterialMessage) + sizeof(MessageHeader);

    totalSize = makeMultiple(msgSize, 256);
    padding = totalSize - msgSize;
    FileMapping::printInfo("*   Material Message Sizes(HDR,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() +
                           (msgSize - sizeof(MessageHeader)) + " " + MString() + padding + " " + MString() + totalSize);

    MessageHeader hdr;
    hdr.nodeType = msginfo.nodeType;
    hdr.messageType = msginfo.msgType;
    hdr.byteTotal = totalSize;
    hdr.byteSize = msgSize - sizeof(MessageHeader);
    hdr.bytePadding = padding;

    return hdr;
    // return 0;
}
MessageHeader FileMapping::createHeaderLight(MessageInfo& msginfo, LightInfo& lInfo)
{
    size_t totalSize;
    size_t msgSize;
    size_t padding;
    size_t infoSize;
    // infoSize = 200 * sizeof(char);
    msgSize = sizeof(LightMessage) + sizeof(MessageHeader);

    totalSize = makeMultiple(msgSize, 256);
    padding = totalSize - msgSize;
    FileMapping::printInfo("*   Light Message Sizes(HDR,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() +
                           (msgSize - sizeof(MessageHeader)) + " " + MString() + padding + " " + MString() + totalSize);

    MessageHeader hdr;
    hdr.nodeType = msginfo.nodeType;
    hdr.messageType = msginfo.msgType;
    hdr.byteTotal = totalSize;
    hdr.byteSize = msgSize - sizeof(MessageHeader);
    hdr.bytePadding = padding;

    return hdr;
    // return 0;
}
MessageHeader FileMapping::createHeaderRenameDelete(MessageInfo& msginfo)
{
    size_t totalSize;
    size_t msgSize;
    size_t padding;
    msgSize = sizeof(MessageHeader) + sizeof(RenameDeleteMessage);
    totalSize = makeMultiple(msgSize, 256);
    padding = totalSize - msgSize;
    msgSize = msgSize - sizeof(MessageHeader);

    MessageHeader hdr;
    hdr.nodeType = msginfo.nodeType;
    hdr.messageType = msginfo.msgType;
    hdr.byteTotal = totalSize;
    hdr.byteSize = msgSize;
    hdr.bytePadding = padding;
    // FileMapping::printWarning(MString() + msginfo.nodeType);
    return hdr;
}
// MessageHeader FileMapping::createHeaderDelete(std::string name1, MessageInfo& msginfo)
//{
//	return MessageHeader();
//}

MeshMessage FileMapping::createMessageMesh(MessageInfo& msginfo, MeshInfo& mInfo)
{
    MeshMessage msg;

    int nodeNameLength = msginfo.nodeName.length();
    int transformNameLength = mInfo.transformName.length();
    int materialNameLength = mInfo.materialName.length();
    if(nodeNameLength <= 100)
    {
        for(int i = 0; i < nodeNameLength; i++)
        {
            msg.objectName[i] = msginfo.nodeName[i];
        }
        // msg.nodeName[nodeNameLength] = (char)"\0";
        msg.objectName[nodeNameLength] = '\0';
    }
    else
    {
        MGlobal::displayError("Node name too long!");
    }
    if(transformNameLength <= 100)
    {
        for(int i = 0; i < transformNameLength; i++)
        {
            msg.transformName[i] = mInfo.transformName[i];
        }
        // msg.nodeName[nodeNameLength] = (char)"\0";
        msg.transformName[transformNameLength] = '\0';
    }
    else
    {
        MGlobal::displayError("Transform name too long!");
    }
    if(materialNameLength <= 100)
    {
        for(int i = 0; i < materialNameLength; i++)
        {
            msg.materialName[i] = mInfo.materialName[i];
        }
        // msg.nodeName[nodeNameLength] = (char)"\0";
        msg.materialName[materialNameLength] = '\0';
    }
    else
    {
        MGlobal::displayError("Material name too long!");
    }
    msg.meshData = mInfo.meshData;
    msg.materialID = mInfo.materialID;
    msg.meshID = mInfo.meshID;
    // FileMapping::printInfo(MString() + msg.meshData.indCount);
    return msg;
}
TransformMessage FileMapping::createMessageTransform(MessageInfo& msginfo, TransformInfo& tInfo)
{
    TransformMessage outMsg;

    int nodeNameLength = msginfo.nodeName.length();
    int parentNameLength = tInfo.parentName.length();
    if(nodeNameLength <= 100)
    {
        for(int i = 0; i < nodeNameLength; i++)
        {
            outMsg.nodeName[i] = msginfo.nodeName[i];
        }
        outMsg.nodeName[nodeNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Node name too long!");
    }
    if(parentNameLength <= 100)
    {
        for(int i = 0; i < parentNameLength; i++)
        {
            outMsg.parentName[i] = tInfo.parentName[i];
        }
        outMsg.parentName[parentNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Transform name too long!");
    }
    outMsg.transformData = tInfo.transformData;


    return outMsg;
}
CameraMessage FileMapping::createMessageCamera(MessageInfo& msginfo, CameraInfo& cInfo)
{
    CameraMessage outMsg;

    int nodeNameLength = msginfo.nodeName.length();
    int parentNameLength = cInfo.transformName.length();
    if(nodeNameLength <= 100)
    {
        for(int i = 0; i < nodeNameLength; i++)
        {
            outMsg.nodeName[i] = msginfo.nodeName[i];
        }
        // FileMapping::printInfo("Node name added!");
        outMsg.nodeName[nodeNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Node name too long!");
    }
    if(parentNameLength <= 100)
    {
        for(int i = 0; i < parentNameLength; i++)
        {
            outMsg.transformName[i] = cInfo.transformName[i];
        }
        outMsg.transformName[parentNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Transform name too long!");
    }
    outMsg.cameraData = cInfo.cameraData;
    return outMsg;
}
MaterialMessage FileMapping::createMessageMaterial(MessageInfo& msginfo, MaterialInfo& mInfo)
{
    MaterialMessage outMsg;

    int nodeNameLength = msginfo.nodeName.length();
    int diffTexLength = mInfo.diffuseTexturePath.length();
    int glowTexLength = mInfo.glowTexturePath.length();
    int specTexLength = mInfo.specTexturePath.length();

    if(nodeNameLength <= 100)
    {
        for(int i = 0; i < nodeNameLength; i++)
        {
            outMsg.nodeName[i] = msginfo.nodeName[i];
        }
        // FileMapping::printInfo("Node name added!");
        outMsg.nodeName[nodeNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Node name too long!");
    }

    // Fill Texture path names
    strcpy(outMsg.diffuseTexturePath, mInfo.diffuseTexturePath.c_str());
    strcpy(outMsg.glowTexturePath, mInfo.glowTexturePath.c_str());
    strcpy(outMsg.specTexturePath, mInfo.specTexturePath.c_str());
    strcpy(outMsg.bumpTexturePath, mInfo.bumpTexturePath.c_str());
    FileMapping::printInfo("GLOW TEX: " + MString(outMsg.glowTexturePath));

    // memcpy(&outMsg.diffuseTexturePath, &mInfo.diffuseTexturePath, sizeof(char) * 100);

    FileMapping::printInfo("TEXTURE PATH: " + MString(outMsg.diffuseTexturePath));

    outMsg.data = mInfo.matData;

    return outMsg;
}
LightMessage FileMapping::createMessageLight(MessageInfo& msgInfo, LightInfo& lInfo)
{
    LightMessage outMsg;

    int nodeNameLength = msgInfo.nodeName.length();
    int parentNameLength = lInfo.transformName.length();
    if(nodeNameLength <= 100)
    {
        for(int i = 0; i < nodeNameLength; i++)
        {
            outMsg.nodeName[i] = msgInfo.nodeName[i];
        }
        // FileMapping::printInfo("Node name added!");
        outMsg.nodeName[nodeNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Node name too long!");
    }
    if(parentNameLength <= 100)
    {
        for(int i = 0; i < parentNameLength; i++)
        {
            outMsg.transformName[i] = lInfo.transformName[i];
        }
        outMsg.transformName[parentNameLength] = '\0';
    }
    else
    {
        FileMapping::printError("* Transform name too long!");
    }
    outMsg.lightData = lInfo.lightData;
    // FileMapping::printInfo(MString() + outMsg.meshData->indCount);

    return outMsg;
}
RenameDeleteMessage FileMapping::createMessageRenameDelete(MessageInfo& msgInfo, RenameDeleteInfo& info)
{
    RenameDeleteMessage outMsg;
    size_t name1Length = info.nodeName1.length();
    size_t name2Length = info.nodeName2.length();

    for(int i = 0; i < name1Length; i++)
    {
        outMsg.nodeName1[i] = info.nodeName1[i];
    }
    outMsg.nodeName1[name1Length] = 0;

    for(int i = 0; i < name2Length; i++)
    {
        outMsg.nodeName2[i] = info.nodeName2[i];
    }
    outMsg.nodeName2[name2Length] = 0;

    FileMapping::printInfo(MString(outMsg.nodeName2) + " " + MString(outMsg.nodeName1));
    return outMsg;
}
// RenameDeleteMessage FileMapping::createMessageDelete(std::string name1)
//{
//	return RenameDeleteMessage();
//}

size_t FileMapping::makeMultiple(size_t size, size_t multiple)
{
    size_t modValue;
    modValue = size % multiple;
    // std::cout << "\nModvalue: " << modValue << "\n";
    if(modValue == 0)
    {
        return size;
    }

    return size + multiple - modValue;
}

void FileMapping::PrintFileMapInfo(bool isPost)
{
    std::string str;
    if(isPost)
    {
        str = "POST";
    }
    else
    {
        str = "PRE";
    }
    FileMapping::printInfo("*** ! Infovalues " + MString(str.c_str()) + " message(HEAD, TAIL, NAM, SIZE): " + MString() +
                           fileMapInfo.head_ByteOffset + " " + MString() + fileMapInfo.tail_ByteOffset + " " + MString() +
                           fileMapInfo.non_accessmemoryOffset + " " + MString() + fileMapInfo.messageFilemap_Size);

    return;
}

std::string FileMapping::GetLastErrorAsString()
{
    // Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) return std::string(); // No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    // Free the buffer.
    LocalFree(messageBuffer);

    return message;
}