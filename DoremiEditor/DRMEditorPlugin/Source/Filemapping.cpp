#include <DoremiEditor\DRMEditorPlugin\Include\Filemapping.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\ApplicationContext.hpp>
#include <DoremiEditor\DRMEditorPlugin\Include\MessageBuilder.hpp>
namespace DoremiEditor
{
    namespace Plugin
    {
        Filemapping::Filemapping()
        {
            m_localHead = 0;
            m_localTail = 0;
            m_messageMapSize = 1024 * 1024 * 10;
            m_infoMapSize = 256;
            m_isActive = false;
            m_chunkSize = 256;
            m_outFileName = "drmMapOut";
        }
        Filemapping::~Filemapping()
        {
            // if (m_isActive)
            //{
            CloseFilemaps();
            //}
        }

        void Filemapping::CreateFilemaps(int p_filemapSize)
        {
            try
            {
                PrintInfo(MString() + m_messageMapSize);
                PrintWarning("REACHED FILEMAP CREATION FUNCTION");
                m_messageBuilder = ApplicationContext::GetInstance().GetMessageBuilder();
                m_mutex.Create("__info_Mutex__");
                SECURITY_ATTRIBUTES security;
                security.lpSecurityDescriptor = nullptr;

                // INITIALIZE INFO FILEMAP
                bool t_infoMapStatus = true;
                f_infoFilemap =
                    CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, (DWORD)0, m_infoMapSize, (LPCWSTR) "Global\\infoFileMap");

                f_infoMapData = MapViewOfFile(f_infoFilemap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
                if(f_infoFilemap == NULL)
                {
                    PrintWarning("Couldn't create infofilemap");
                    t_infoMapStatus = false;
                }

                if(GetLastError() == ERROR_ALREADY_EXISTS)
                {
                    PrintInfo("Infofilemap exists, you get a handle to it!");
                    GetInfoMapValues();
                    if(p_filemapSize != 0)
                    {
                        SetInfoMapValues(0, 0, 256, p_filemapSize);
                    }
                }
                else
                { // first, sätter de första värdena på filemapinfon
                    PrintWarning("Info filemap not found. Creating new (Warning, not recommended)");
                    // m_isActive = true;
                    SetInfoMapValues(0, 0, 256, p_filemapSize);
                }
                int o = static_cast<int>(m_messageMapSize);
                if(o > 0)
                {
                    PrintDebug("Message Filemap SIZE: " + MString() + o);
                }
                PrintInfo(MString() + p_filemapSize);


                // m_nonAccessMemoryOffset = m_fileMapInfoStructured.non_accessmemoryOffset;

                // INITIALIZE MESSAGE FILEMAP
                bool t_messageMapStatus = true;
                f_messageFilemap =
                    CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, (DWORD)0, m_messageMapSize, (LPCWSTR) "Global\\messageFileMap");

                f_messageMapData = MapViewOfFile(f_messageFilemap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

                if(f_messageFilemap == NULL)
                {
                    PrintWarning("Couldn't create message filemap");
                    t_messageMapStatus = false;
                }
                if(GetLastError() == ERROR_ALREADY_EXISTS)
                {
                    PrintInfo("Filemap exists, you get a handle to it!");
                    // m_isActive = true;
                }
                else
                {
                    PrintInfo("Could not find Message Filemap! Creating new (Warning, not recommended)");
                    // m_isActive = true;
                }
                if(t_infoMapStatus && t_messageMapStatus)
                {
                    m_isActive = true;
                    SetExportFileName();
                    PrintInfo("FileMapSize: " + MString() + m_infoMapSize + " " + MString() + m_fileMapInfoStructured.non_accessmemoryOffset);
                    PrintInfo("***** FILEMAP LOADED *****");
                }
                else
                {
                    PrintWarning("Filemaps not loaded!");
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void Filemapping::CloseFilemaps()
        {
            try
            {
                bool result = UnmapViewOfFile((LPCVOID)f_messageMapData);
                if(GetLastError() != 0)
                {
                    PrintError("Error closing Message Filemap");
                }
                if(result != 0)
                {
                    PrintInfo("Message Filemap closed");
                }
                result = CloseHandle(f_messageFilemap);
                if(GetLastError() != 0)
                {
                    PrintError("Error closing Message Filemap");
                }
                if(result != 0)
                {
                    PrintInfo("Message Filemap Handle closed");
                }
                result = UnmapViewOfFile((LPCVOID)f_infoMapData);
                if(GetLastError() != 0)
                {
                    PrintError("Error closing Message Filemap");
                }
                if(result != 0)
                {
                    PrintInfo("Information Filemap closed");
                }
                result = CloseHandle(f_infoFilemap);
                if(GetLastError() != 0)
                {
                    PrintError("Error closing Message Filemap");
                }
                if(result != 0)
                {
                    PrintInfo("Information Filemap Handle closed");
                }
                m_isActive = false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        bool Filemapping::GetFilemapStatus()
        {
            try
            {
                return m_isActive;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void Filemapping::SetExportFileName(std::string p_fileName)
        {
            try
            {
                if(p_fileName.length() > 1)
                {
                    size_t t_fileNameLength = p_fileName.length();
                    m_outFileName = p_fileName;
                    strcpy(m_fileMapInfoStructured.outFileName, p_fileName.c_str());
                    m_fileMapInfoStructured.outFileName[t_fileNameLength] = 0;
                }
                else
                {
                    size_t t_fileNameLength = m_outFileName.length();
                    strcpy(m_fileMapInfoStructured.outFileName, m_outFileName.c_str());
                    m_fileMapInfoStructured.outFileName[t_fileNameLength] = 0;
                }

                PrintInfo(MString(m_fileMapInfoStructured.outFileName));

                SetInfoMapValues();
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void Filemapping::SetInfoMapValues(int p_head, int p_tail, int p_safetyMemorySize, int p_messageFilemapSize, std::string p_fileName)
        {
            try
            {
                // memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                if(p_head >= 0)
                {
                    m_fileMapInfoStructured.head_ByteOffset = static_cast<size_t>(p_head);
                    m_localHead = static_cast<size_t>(p_head);
                }
                else
                {
                    m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                }

                if(p_tail >= 0)
                {
                    m_fileMapInfoStructured.tail_ByteOffset = static_cast<size_t>(p_tail);
                    m_localTail = static_cast<size_t>(p_tail);
                }
                else
                {
                    m_fileMapInfoStructured.tail_ByteOffset = m_localTail;
                }

                if(p_safetyMemorySize >= 0)
                {
                    m_fileMapInfoStructured.non_accessmemoryOffset = static_cast<size_t>(p_safetyMemorySize);
                    m_nonAccessMemoryOffset = static_cast<size_t>(p_safetyMemorySize);
                }
                else
                {
                    m_fileMapInfoStructured.non_accessmemoryOffset = m_nonAccessMemoryOffset;
                }
                if(p_messageFilemapSize >= 0)
                {
                    m_fileMapInfoStructured.messageFilemap_Size = static_cast<size_t>(p_messageFilemapSize);
                    m_messageMapSize = static_cast<size_t>(p_messageFilemapSize);
                }
                else
                {
                    m_fileMapInfoStructured.messageFilemap_Size = m_messageMapSize;
                }

                // m_fileMapInfoStructured.

                if(m_isActive)
                {
                    while(m_mutex.Lock(1000) == false)
                        Sleep(10);
                    memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                    m_mutex.Unlock();
                }

                /*m_fileMapInfoStructured.head_ByteOffset = p_head;
                m_fileMapInfoStructured.tail_ByteOffset = p_tail;
                m_fileMapInfoStructured.non_accessmemoryOffset = p_safetyMemorySize;
                m_fileMapInfoStructured.messageFilemap_Size = p_messageFilemapSize;*/
                // m_nonAccessMemoryOffset = p_safetyMemorySize;
                // m_messageMapSize = p_messageFilemapSize;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        void Filemapping::GetInfoMapValues()
        {
            try
            {
                while(m_mutex.Lock(1000) == false)
                    Sleep(10);
                memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData,
                       sizeof(FilemapInfo)); // får inget värde!!!!!!! default värdet är fortfarande kvar
                if(m_fileMapInfoStructured.messageFilemap_Size != 0)
                {
                    m_messageMapSize = m_fileMapInfoStructured.messageFilemap_Size;
                }
                if(m_fileMapInfoStructured.non_accessmemoryOffset != 0)
                {
                    m_nonAccessMemoryOffset = m_fileMapInfoStructured.non_accessmemoryOffset;
                }

                m_localHead = m_fileMapInfoStructured.head_ByteOffset;
                m_localTail = m_fileMapInfoStructured.tail_ByteOffset;
                m_mutex.Unlock();
                PrintDebug("MessageFMMemorySize: " + MString() + m_fileMapInfoStructured.messageFilemap_Size);
                PrintDebug("Nonaccess: " + MString() + m_fileMapInfoStructured.non_accessmemoryOffset);
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        void Filemapping::PrintFilemapInfo(bool p_isPostMessage, bool p_onlyFileName)
        {
            try
            {
                if(!p_onlyFileName)
                {
                    std::string str;
                    if(p_isPostMessage)
                    {
                        str = "POST";
                    }
                    else
                    {
                        str = "PRE";
                    }
                    PrintDebug("*** ! Infovalues " + MString(str.c_str()) + " message(HEAD, TAIL, NAM, SIZE): " + MString() +
                               m_fileMapInfoStructured.head_ByteOffset + " " + MString() + m_fileMapInfoStructured.tail_ByteOffset + " " + MString() +
                               m_fileMapInfoStructured.non_accessmemoryOffset + " " + MString() + m_fileMapInfoStructured.messageFilemap_Size);
                }
                else
                {
                    // nothing
                }
                PrintDebug("*** ! Export File Name " + MString(m_outFileName.c_str()) + ".drm");
                return;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        MessageHeader Filemapping::CreateHeaderTransform(MessageInfo& p_messageInfo)
        {

            try
            {
                MessageHeader o_header;
                size_t totalSize;
                size_t msgSize;
                size_t padding;
                // infoSize = 200 * sizeof(char);
                msgSize = sizeof(TransformMessage) + sizeof(MessageHeader);

                totalSize = MakeMultiple(msgSize);
                padding = totalSize - msgSize;
                /*PrintDebug("*   Transform Message Sizes(p_header,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() + (msgSize -
                   sizeof(MessageHeader))
                    + " " + MString() + padding + " " + MString() + totalSize);*/

                o_header.nodeType = static_cast<int>(p_messageInfo.nodeType);
                o_header.messageType = static_cast<int>(p_messageInfo.msgType);
                o_header.byteTotal = totalSize;
                o_header.byteSize = msgSize - sizeof(MessageHeader);
                o_header.bytePadding = padding;
                return o_header;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MessageHeader Filemapping::CreateHeaderMesh(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                size_t t_messageSize;
                size_t t_totalSize;
                size_t t_padding;
                t_messageSize = m_messageBuilder->GetMeshDataSize(p_messageInfo.nodeName);

                t_totalSize = MakeMultiple(t_messageSize + sizeof(MessageHeader));
                t_padding = t_totalSize - t_messageSize;

                MessageHeader o_header;
                o_header.nodeType = static_cast<int>(p_messageInfo.nodeType);
                o_header.messageType = static_cast<int>(p_messageInfo.msgType);
                o_header.byteTotal = t_totalSize;
                o_header.byteSize = t_messageSize - sizeof(MessageHeader);
                o_header.bytePadding = t_padding;
                return o_header;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MessageHeader Filemapping::CreateHeaderCamera(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                size_t totalSize;
                size_t msgSize;
                size_t padding;
                size_t infoSize;
                // infoSize = 200 * sizeof(char);
                msgSize = sizeof(CameraMessage) + sizeof(MessageHeader);

                totalSize = MakeMultiple(msgSize);
                padding = totalSize - msgSize;
                /*PrintDebug("*   Camera Message Sizes(p_header,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() + (msgSize -
                   sizeof(MessageHeader))
                    + " " + MString() + padding + " " + MString() + totalSize);*/
                MessageHeader o_header;
                o_header.nodeType = static_cast<int>(p_messageInfo.nodeType);
                o_header.messageType = static_cast<int>(p_messageInfo.msgType);
                o_header.byteTotal = totalSize;
                o_header.byteSize = msgSize - sizeof(MessageHeader);
                o_header.bytePadding = padding;
                return o_header;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MessageHeader Filemapping::CreateHeaderLight(MessageInfo& p_messageInfo)
        {
            try
            {
                size_t totalSize;
                size_t msgSize;
                size_t padding;
                size_t infoSize;
                // infoSize = 200 * sizeof(char);
                msgSize = sizeof(LightMessage) + sizeof(MessageHeader);

                totalSize = MakeMultiple(msgSize);
                padding = totalSize - msgSize;
                /*PrintDebug("*   Light Message Sizes(p_header,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() + (msgSize -
                   sizeof(MessageHeader))
                    + " " + MString() + padding + " " + MString() + totalSize);*/
                MessageHeader o_header;
                o_header.nodeType = static_cast<int>(p_messageInfo.nodeType);
                o_header.messageType = static_cast<int>(p_messageInfo.msgType);
                o_header.byteTotal = totalSize;
                o_header.byteSize = msgSize - sizeof(MessageHeader);
                o_header.bytePadding = padding;
                return o_header;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MessageHeader Filemapping::CreateHeaderMaterial(MessageInfo& p_messageInfo)
        {
            try
            {
                size_t totalSize;
                size_t msgSize;
                size_t padding;
                size_t infoSize;
                // infoSize = 200 * sizeof(char);
                msgSize = sizeof(MaterialMessage) + sizeof(MessageHeader);

                totalSize = MakeMultiple(msgSize);
                padding = totalSize - msgSize;
                /*PrintDebug("*   Material Message Sizes(p_header,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() + (msgSize -
                   sizeof(MessageHeader))
                    + " " + MString() + padding + " " + MString() + totalSize);*/
                MessageHeader o_header;
                o_header.nodeType = static_cast<int>(p_messageInfo.nodeType);
                o_header.messageType = static_cast<int>(p_messageInfo.msgType);
                o_header.byteTotal = totalSize;
                o_header.byteSize = msgSize - sizeof(MessageHeader);
                o_header.bytePadding = padding;
                return o_header;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MessageHeader Filemapping::CreateHeaderRenameDelete(MessageInfo& p_messageInfo)
        {
            try
            {
                size_t totalSize;
                size_t msgSize;
                size_t padding;
                size_t infoSize;
                // infoSize = 200 * sizeof(char);
                msgSize = sizeof(RenameDeleteMessage) + sizeof(MessageHeader);

                totalSize = MakeMultiple(msgSize);
                padding = totalSize - msgSize;
                /*PrintDebug("*   Rename / Delete Message Sizes(p_header,MSG,PDG,TOT): " + MString() + sizeof(MessageHeader) + " " + MString() +
                   (msgSize - sizeof(MessageHeader))
                    + " " + MString() + padding + " " + MString() + totalSize);*/
                MessageHeader o_header;
                o_header.nodeType = static_cast<int>(p_messageInfo.nodeType);
                o_header.messageType = static_cast<int>(p_messageInfo.msgType);
                o_header.byteTotal = totalSize;
                o_header.byteSize = msgSize - sizeof(MessageHeader);
                o_header.bytePadding = padding;
                return o_header;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        size_t Filemapping::FindWriteConfig(MessageHeader& p_header)
        {
            try
            {
                size_t o_config = 0;

                while(m_mutex.Lock(1000) == false)
                    Sleep(10);
                memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                m_localHead = m_fileMapInfoStructured.head_ByteOffset;
                m_localTail = m_fileMapInfoStructured.tail_ByteOffset;
                m_mutex.Unlock();

                if(m_localHead >= m_localTail)
                {
                    if(p_header.byteSize + p_header.bytePadding + m_localHead + m_nonAccessMemoryOffset <= m_messageMapSize)
                    {
                        p_header.msgConfig = 0;
                        PrintDebug("MSG Config (CAN WRITE NORMALLY)");
                        return 1;
                    }
                    else if(m_localHead + sizeof(MessageHeader) <= m_messageMapSize && MakeMultiple(p_header.byteSize) + m_nonAccessMemoryOffset <= m_localTail)
                    {
                        PrintDebug("MSG Config (CAN WRITE WITH SPLIT)");
                        size_t tempTotal;
                        p_header.msgConfig = 1;
                        tempTotal = MakeMultiple(p_header.byteSize);
                        p_header.bytePadding = tempTotal - p_header.byteSize;
                        return 2;
                    }
                    else if(p_header.byteTotal + m_nonAccessMemoryOffset <= m_localTail)
                    {
                        p_header.msgConfig = 2;
                        return 3;
                    }
                }
                else
                {
                    if(p_header.byteTotal + m_localHead + m_nonAccessMemoryOffset <= m_localTail)
                    {
                        p_header.msgConfig = 0;
                        PrintDebug("MSG Config (CAN WRITE NORMALLY)");
                        return 1;
                    }
                }
                PrintDebug("MSG Config (CANNOT WRITE)");
                return o_config;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        bool Filemapping::TrySendTransform(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                if(m_isActive)
                {
                    PrintDebug("TRYING TO SEND TRANSFORM MESSAGE FOR: " + MString(p_messageInfo.nodeName.c_str()));
                    MessageHeader t_header = CreateHeaderTransform(p_messageInfo);
                    size_t t_cfg = FindWriteConfig(t_header);
                    if(t_cfg != 0)
                    {
                        TransformMessage t_message = CreateMessageTransform(p_messageInfo);
                        if(SendTransformMessage(t_header, t_message, t_cfg) == true)
                        {
                            PrintMessage(t_header, p_messageInfo.nodeName, t_message.parentName);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::TrySendMesh(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                if(m_isActive)
                {
                    PrintDebug("TRYING TO SEND MESH MESSAGE FOR: " + MString(p_messageInfo.nodeName.c_str()));
                    MessageHeader t_header = CreateHeaderTransform(p_messageInfo);
                    size_t t_cfg = FindWriteConfig(t_header);
                    if(t_cfg != 0)
                    {
                        MeshMessage t_message = CreateMessageMesh(p_messageInfo);
                        if(SendMeshMessage(t_header, t_message, t_cfg) == true)
                        {
                            PrintMessage(t_header, p_messageInfo.nodeName, t_message.transformName[0].transformNames, t_message.transformCount);
                            delete[] t_message.data.uv;
                            delete[] t_message.data.triPerFace;
                            delete[] t_message.data.triIndices;
                            delete[] t_message.data.norIndices;
                            delete[] t_message.data.UVIndices;
                            return true;
                        }
                        else
                        {
                            delete[] t_message.data.uv;
                            delete[] t_message.data.triPerFace;
                            delete[] t_message.data.triIndices;
                            delete[] t_message.data.norIndices;
                            delete[] t_message.data.UVIndices;
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::TrySendCamera(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                if(m_isActive)
                {
                    PrintDebug("TRYING TO SEND CAMERA MESSAGE FOR: " + MString(p_messageInfo.nodeName.c_str()));
                    MessageHeader t_header = CreateHeaderCamera(p_messageInfo);
                    size_t t_cfg = FindWriteConfig(t_header);
                    if(t_cfg != 0)
                    {
                        CameraMessage t_message = CreateMessageCamera(p_messageInfo);
                        if(SendCameraMessage(t_header, t_message, t_cfg) == true)
                        {
                            PrintMessage(t_header, p_messageInfo.nodeName, t_message.transformName);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::TrySendLight(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                if(m_isActive)
                {
                    PrintDebug("TRYING TO SEND LIGHT MESSAGE FOR: " + MString(p_messageInfo.nodeName.c_str()));
                    MessageHeader t_header = CreateHeaderLight(p_messageInfo);
                    size_t t_cfg = FindWriteConfig(t_header);
                    if(t_cfg != 0)
                    {
                        LightMessage t_message = CreateMessageLight(p_messageInfo);
                        if(SendLightMessage(t_header, t_message, t_cfg) == true)
                        {
                            PrintMessage(t_header, p_messageInfo.nodeName, t_message.transformName);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::TrySendMaterial(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                if(m_isActive)
                {
                    PrintDebug("TRYING TO SEND MESSAGE FOR: " + MString(p_messageInfo.nodeName.c_str()));
                    MessageHeader t_header = CreateHeaderMaterial(p_messageInfo);
                    size_t t_cfg = FindWriteConfig(t_header);
                    if(t_cfg != 0)
                    {
                        MaterialMessage t_message = CreateMessageMaterial(p_messageInfo);
                        if(SendMaterialMessage(t_header, t_message, t_cfg) == true)
                        {
                            PrintMessage(t_header, p_messageInfo.nodeName);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::TrySendRenameDelete(MessageInfo& p_messageInfo)
        {
            try
            {
                bool result = false;
                if(m_isActive)
                {
                    PrintDebug("TRYING TO SEND MESSAGE FOR: " + MString(p_messageInfo.nodeName.c_str()));
                    MessageHeader t_header = CreateHeaderRenameDelete(p_messageInfo);
                    size_t t_cfg = FindWriteConfig(t_header);
                    if(t_cfg != 0)
                    {
                        RenameDeleteMessage t_message = CreateMessageRenameDelete(p_messageInfo);
                        if(SendRenameDeleteMessage(t_header, t_message, t_cfg) == true)
                        {
                            PrintMessage(t_header, p_messageInfo.nodeName, p_messageInfo.oldName);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }

        TransformMessage Filemapping::CreateMessageTransform(MessageInfo& p_messageInfo)
        {
            try
            {
                TransformMessage o_messageData;
                TransformInfo t_transformInfo = m_messageBuilder->GetTransformData(p_messageInfo.nodeName);
                int nodeNameLength = p_messageInfo.nodeName.length();
                int parentNameLength = t_transformInfo.parentName.length();

                if(nodeNameLength <= 100)
                {
                    for(int i = 0; i < nodeNameLength; i++)
                    {
                        o_messageData.nodeName[i] = p_messageInfo.nodeName[i];
                    }
                    o_messageData.nodeName[nodeNameLength] = '\0';
                }
                else
                {
                    PrintError("* Node name too long!");
                }
                if(parentNameLength <= 100)
                {
                    for(int i = 0; i < parentNameLength; i++)
                    {
                        o_messageData.parentName[i] = t_transformInfo.parentName[i];
                    }
                    o_messageData.parentName[parentNameLength] = '\0';
                }
                else
                {
                    PrintError("* Transform name too long!");
                }
                o_messageData.data = t_transformInfo.data;
                return o_messageData;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MeshMessage Filemapping::CreateMessageMesh(MessageInfo& p_messageInfo)
        {
            try
            {
                MeshMessage o_messageData;
                MeshInfo t_meshInfo = m_messageBuilder->GetMeshData(p_messageInfo.nodeName);
                int t_nodeNameLength = p_messageInfo.nodeName.length();
                int t_materialNameLength = t_meshInfo.materialName.length();
                if(t_nodeNameLength <= 100)
                {
                    for(int i = 0; i < t_nodeNameLength; i++)
                    {
                        o_messageData.nodeName[i] = p_messageInfo.nodeName[i];
                    }
                    // msg.nodeName[nodeNameLength] = (char)"\0";
                    o_messageData.nodeName[t_nodeNameLength] = '\0';
                }
                else
                {
                    MGlobal::displayError("Node name too long!");
                }
                if(t_materialNameLength <= 100)
                {
                    for(int i = 0; i < t_materialNameLength; i++)
                    {
                        o_messageData.materialName[i] = t_meshInfo.materialName[i];
                    }
                    // msg.nodeName[nodeNameLength] = (char)"\0";
                    o_messageData.materialName[t_materialNameLength] = '\0';
                }
                else
                {
                    MGlobal::displayError("Material name too long!");
                }
                o_messageData.transformCount = t_meshInfo.transformCount;
                for(int o = 0; o < o_messageData.transformCount; o++)
                {
                    int transformNameLength = t_meshInfo.transformName[o].length();
                    if(transformNameLength < 100)
                    {
                        NameStruct tempName;
                        for(int i = 0; i < transformNameLength; i++)
                        {
                            tempName.transformNames[i] = t_meshInfo.transformName.at(o)[i];
                        }
                        // msg.nodeName[nodeNameLength] = (char)"\0";
                        tempName.transformNames[transformNameLength] = '\0';
                        o_messageData.transformName.push_back(tempName);
                        PrintDebug("Parent name " + MString() + o + " :" + o_messageData.transformName[o].transformNames);
                    }
                    else
                    {
                        o_messageData.transformCount--;
                        MGlobal::displayError("Transform name too long!");
                    }
                }
                o_messageData.data = t_meshInfo.data;
                o_messageData.materialID = t_meshInfo.materialID;
                o_messageData.meshID = t_meshInfo.meshID;

                return o_messageData;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        CameraMessage Filemapping::CreateMessageCamera(MessageInfo& p_messageInfo)
        {
            try
            {
                CameraMessage o_messageData;
                CameraInfo t_cameraInfo = m_messageBuilder->GetCameraData(p_messageInfo.nodeName);
                int nodeNameLength = p_messageInfo.nodeName.length();
                int parentNameLength = t_cameraInfo.transformName.length();

                if(nodeNameLength <= 100)
                {
                    for(int i = 0; i < nodeNameLength; i++)
                    {
                        o_messageData.nodeName[i] = p_messageInfo.nodeName[i];
                    }
                    o_messageData.nodeName[nodeNameLength] = '\0';
                }
                else
                {
                    PrintError("* Node name too long!");
                }
                if(parentNameLength <= 100)
                {
                    for(int i = 0; i < parentNameLength; i++)
                    {
                        o_messageData.transformName[i] = t_cameraInfo.transformName[i];
                    }
                    o_messageData.transformName[parentNameLength] = '\0';
                }
                else
                {
                    PrintError("* Transform name too long!");
                }
                o_messageData.data = t_cameraInfo.data;
                return o_messageData;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        LightMessage Filemapping::CreateMessageLight(MessageInfo& p_messageInfo)
        {
            try
            {
                LightMessage o_messageData;
                LightInfo t_lightInfo = m_messageBuilder->GetLightData(p_messageInfo.nodeName);
                int nodeNameLength = p_messageInfo.nodeName.length();
                int parentNameLength = t_lightInfo.transformName.length();

                if(nodeNameLength <= 100)
                {
                    for(int i = 0; i < nodeNameLength; i++)
                    {
                        o_messageData.nodeName[i] = p_messageInfo.nodeName[i];
                    }
                    o_messageData.nodeName[nodeNameLength] = '\0';
                }
                else
                {
                    PrintError("* Node name too long!");
                }
                if(parentNameLength <= 100)
                {
                    for(int i = 0; i < parentNameLength; i++)
                    {
                        o_messageData.transformName[i] = t_lightInfo.transformName[i];
                    }
                    o_messageData.transformName[parentNameLength] = '\0';
                }
                else
                {
                    PrintError("* Transform name too long!");
                }
                o_messageData.data = t_lightInfo.data;
                return o_messageData;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        MaterialMessage Filemapping::CreateMessageMaterial(MessageInfo& p_messageInfo)
        {
            try
            {
                MaterialMessage o_messageData;
                MaterialInfo t_materialInfo = m_messageBuilder->GetMaterialData(p_messageInfo.nodeName);
                size_t nodeNameLength = p_messageInfo.nodeName.length();
                size_t diffTexLength = t_materialInfo.diffuseTexturePath.length();
                size_t glowTexLength = t_materialInfo.glowTexturePath.length();
                size_t specTexLength = t_materialInfo.specTexturePath.length();
                size_t bumpTexLength = t_materialInfo.bumpTexturePath.length();
                if(nodeNameLength <= 100)
                {
                    for(int i = 0; i < nodeNameLength; i++)
                    {
                        o_messageData.nodeName[i] = p_messageInfo.nodeName[i];
                    }
                    // FileMapping::printInfo("Node name added!");
                    o_messageData.nodeName[nodeNameLength] = '\0';
                }
                else
                {
                    PrintError("* Node name too long!");
                }

                // Fill Texture path names
                if(diffTexLength > 0)
                {
                    strcpy(o_messageData.diffuseTexturePath, t_materialInfo.diffuseTexturePath.c_str());
                    o_messageData.diffuseTexturePath[diffTexLength] = '\0';
                    PrintDebug("Diffuse TEX: " + MString(o_messageData.diffuseTexturePath));
                }
                else
                {
                    o_messageData.diffuseTexturePath[0] = 0;
                }
                if(glowTexLength > 0)
                {
                    strcpy(o_messageData.glowTexturePath, t_materialInfo.glowTexturePath.c_str());
                    o_messageData.glowTexturePath[glowTexLength] = '\0';
                    PrintDebug("Glow TEX: " + MString(o_messageData.diffuseTexturePath));
                }
                else
                {
                    o_messageData.glowTexturePath[0] = 0;
                }
                if(specTexLength > 0)
                {
                    strcpy(o_messageData.specTexturePath, t_materialInfo.specTexturePath.c_str());
                    o_messageData.specTexturePath[specTexLength] = '\0';
                    PrintDebug("Specular TEX: " + MString(o_messageData.diffuseTexturePath));
                }
                else
                {
                    o_messageData.specTexturePath[0] = 0;
                }
                if(bumpTexLength > 0)
                {
                    strcpy(o_messageData.bumpTexturePath, t_materialInfo.bumpTexturePath.c_str());
                    o_messageData.bumpTexturePath[bumpTexLength] = '\0';
                    PrintDebug("Bump TEX: " + MString(o_messageData.diffuseTexturePath));
                }
                else
                {
                    o_messageData.bumpTexturePath[0] = 0;
                }


                o_messageData.type = t_materialInfo.type;
                o_messageData.data = t_materialInfo.data;

                return o_messageData;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
        RenameDeleteMessage Filemapping::CreateMessageRenameDelete(MessageInfo& p_messageInfo)
        {
            try
            {
                RenameDeleteMessage outMsg;
                size_t name1Length = p_messageInfo.nodeName.length();
                size_t name2Length = p_messageInfo.oldName.length();

                for(int i = 0; i < name1Length; i++)
                {
                    outMsg.nodeName1[i] = p_messageInfo.nodeName[i];
                }
                outMsg.nodeName1[name1Length] = 0;

                for(int i = 0; i < name2Length; i++)
                {
                    outMsg.nodeName2[i] = p_messageInfo.oldName[i];
                }
                outMsg.nodeName2[name2Length] = 0;

                PrintDebug("Created Message (rename/delete): " + MString(outMsg.nodeName2) + " " + MString(outMsg.nodeName1));
                return outMsg;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }

        bool Filemapping::SendTransformMessage(MessageHeader& p_header, TransformMessage& p_data, int p_config)
        {
            try
            {
                int cfg = p_config;
                switch(cfg)
                {
                    case 1:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));

                        m_localHead += sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_data, p_header.byteSize);

                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        if(m_localHead == m_infoMapSize)
                        {
                            m_localHead = 0;
                        }
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 2:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        m_localHead = 0;
                        memcpy((unsigned char*)f_messageMapData, &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 3:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData, &p_header, sizeof(MessageHeader));
                        m_localHead = sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + sizeof(MessageHeader), &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);

                        return true;
                        break;
                }
                return false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::SendMeshMessage(MessageHeader& p_header, MeshMessage& p_data, int p_config)
        {
            try
            {
                int cfg = p_config;
                switch(cfg)
                {
                    case 1:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        size_t tempHead;
                        // tempHead = 0;
                        tempHead = sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data, sizeof(char) * 100);
                        tempHead += sizeof(char) * 100;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.transformCount, sizeof(int));
                        tempHead += sizeof(int);
                        for(int i = 0; i < p_data.transformCount; i++)
                        {
                            memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.transformName[i].transformNames, sizeof(char) * 100);
                            // PrintWarning(MString(p_data.nodeName) + " parent: " + MString(p_data.transformName[i].transformNames));
                            tempHead += sizeof(char) * 100;
                        }

                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.materialName, sizeof(char) * 100);
                        tempHead += sizeof(char) * 100;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.meshID, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.materialID, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.vertCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.normalCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.UVCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.indCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.triCount, sizeof(int));
                        tempHead += sizeof(int);


                        /*memcpy((unsigned char*)f_infoMapData + m_localHead + tempHead, &p_data, sizeof(int) * 2);
                        tempHead += sizeof(int) * 2;
                        memcpy((unsigned char*)f_infoMapData + m_localHead + tempHead, &p_data+sizeof(int)*2, sizeof(char) * 300);
                        tempHead += sizeof(char) * 300;
                        memcpy((unsigned char*)f_infoMapData + m_localHead + tempHead, &p_data+sizeof(int)*2+300, sizeof(int) * 5);
                        tempHead += sizeof(int) * 5;*/
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.vertices, sizeof(float) * 3 * p_data.data.vertCount);
                        tempHead += sizeof(float) * 3 * p_data.data.vertCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.normals, sizeof(float) * 3 * p_data.data.normalCount);
                        tempHead += sizeof(float) * 3 * p_data.data.normalCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.uv, sizeof(float) * 2 * p_data.data.UVCount);
                        tempHead += sizeof(float) * 2 * p_data.data.UVCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.triIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;

                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.norIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.UVIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.triPerFace, sizeof(int) * p_data.data.triCount);
                        tempHead += sizeof(int) * p_data.data.triCount;


                        tempHead += p_header.bytePadding;

                        m_localHead += tempHead;
                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        if(m_localHead == m_messageMapSize)
                        {
                            m_localHead = 0;
                        }
                        m_fileMapInfoStructured.head_ByteOffset += p_header.byteTotal;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 2:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        m_localHead = 0;
                        tempHead = sizeof(MessageHeader);


                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data, sizeof(char) * 100);
                        tempHead += sizeof(char) * 100;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.transformCount, sizeof(int));
                        tempHead += sizeof(int);
                        for(int i = 0; i < p_data.transformCount; i++)
                        {
                            memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.transformName[i].transformNames, sizeof(char) * 100);
                            tempHead += sizeof(char) * 100;
                        }
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.materialName, sizeof(char) * 100);
                        tempHead += sizeof(char) * 100;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.meshID, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.materialID, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.vertCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.normalCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.UVCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.indCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.triCount, sizeof(int));
                        tempHead += sizeof(int);

                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.vertices, sizeof(float) * 3 * p_data.data.vertCount);
                        tempHead += sizeof(float) * 3 * p_data.data.vertCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.normals, sizeof(float) * 3 * p_data.data.normalCount);
                        tempHead += sizeof(float) * 3 * p_data.data.normalCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.uv, sizeof(float) * 2 * p_data.data.UVCount);
                        tempHead += sizeof(float) * 2 * p_data.data.UVCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.triIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.norIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.UVIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.triPerFace, sizeof(int) * p_data.data.triCount);
                        tempHead += sizeof(int) * p_data.data.triCount;

                        PrintDebug("***** (CFG " + MString() + cfg + ") " + MString() + (tempHead + p_header.bytePadding) + " " + MString() + p_header.byteTotal);
                        tempHead += p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 3:
                        PrintFilemapInfo(false);
                        m_localHead = 0;
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        tempHead = sizeof(MessageHeader);

                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data, sizeof(char) * 100);
                        tempHead += sizeof(char) * 100;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.transformCount, sizeof(int));
                        tempHead += sizeof(int);
                        for(int i = 0; i < p_data.transformCount; i++)
                        {
                            memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.transformName[i].transformNames, sizeof(char) * 100);
                            tempHead += sizeof(char) * 100;
                        }
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.materialName, sizeof(char) * 100);
                        tempHead += sizeof(char) * 100;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.meshID, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.materialID, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.vertCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.normalCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.UVCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.indCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, &p_data.data.triCount, sizeof(int));
                        tempHead += sizeof(int);
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.vertices, sizeof(float) * 3 * p_data.data.vertCount);
                        tempHead += sizeof(float) * 3 * p_data.data.vertCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.normals, sizeof(float) * 3 * p_data.data.normalCount);
                        tempHead += sizeof(float) * 3 * p_data.data.normalCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.uv, sizeof(float) * 2 * p_data.data.UVCount);
                        tempHead += sizeof(float) * 2 * p_data.data.UVCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.triIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.norIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.UVIndices, sizeof(int) * p_data.data.indCount);
                        tempHead += sizeof(int) * p_data.data.indCount;
                        memcpy((unsigned char*)f_messageMapData + m_localHead + tempHead, p_data.data.triPerFace, sizeof(int) * p_data.data.triCount);
                        tempHead += sizeof(int) * p_data.data.triCount;

                        PrintDebug("***** (CFG " + MString() + cfg + ") " + MString() + (tempHead + p_header.bytePadding) + " " + MString() + p_header.byteTotal);
                        tempHead += p_header.bytePadding;
                        m_localHead += tempHead;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;
                }


                return false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::SendCameraMessage(MessageHeader& p_header, CameraMessage& p_data, int p_config)
        {
            try
            {
                int cfg = p_config;
                switch(cfg)
                {
                    case 1:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));

                        m_localHead += sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_data, p_header.byteSize);

                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        if(m_localHead == m_messageMapSize)
                        {
                            m_localHead = 0;
                        }
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 2:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        m_localHead = 0;
                        memcpy((unsigned char*)f_messageMapData, &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 3:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData, &p_header, sizeof(MessageHeader));
                        m_localHead = sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + sizeof(MessageHeader), &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;
                }
                return false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::SendLightMessage(MessageHeader& p_header, LightMessage& p_data, int p_config)
        {
            try
            {
                int cfg = p_config;
                // FileMapping::printInfo("\n\nWRITE LIGHT " + MString(ldata.nodeName) + " \n\n");
                switch(cfg)
                {
                    case 1:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));

                        m_localHead += sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_data, p_header.byteSize);

                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        if(m_localHead == m_messageMapSize)
                        {
                            m_localHead = 0;
                        }
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 2:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        m_localHead = 0;
                        memcpy((unsigned char*)f_messageMapData, &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 3:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData, &p_header, sizeof(MessageHeader));
                        m_localHead = sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + sizeof(MessageHeader), &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;


                        break;
                }
                return false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::SendMaterialMessage(MessageHeader& p_header, MaterialMessage& p_data, int p_config)
        {
            try
            {
                int cfg = p_config;
                switch(cfg)
                {
                    case 1:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));

                        m_localHead += sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_data, p_header.byteSize);

                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        if(m_localHead == m_messageMapSize)
                        {
                            m_localHead = 0;
                        }
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 2:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        m_localHead = 0;
                        memcpy((unsigned char*)f_messageMapData, &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 3:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData, &p_header, sizeof(MessageHeader));
                        m_localHead = sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + sizeof(MessageHeader), &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;

                        break;
                }
                return false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }
        bool Filemapping::SendRenameDeleteMessage(MessageHeader& p_header, RenameDeleteMessage& p_data, int p_config)
        {
            try
            {
                int cfg = p_config;
                switch(cfg)
                {
                    case 1:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));

                        m_localHead += sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_data, p_header.byteSize);

                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        if(m_localHead == m_messageMapSize)
                        {
                            m_localHead = 0;
                        }
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 2:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData + m_localHead, &p_header, sizeof(MessageHeader));
                        m_localHead = 0;
                        memcpy((unsigned char*)f_messageMapData, &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;
                        break;

                    case 3:
                        PrintFilemapInfo(false);
                        memcpy((unsigned char*)f_messageMapData, &p_header, sizeof(MessageHeader));
                        m_localHead = sizeof(MessageHeader);
                        memcpy((unsigned char*)f_messageMapData + sizeof(MessageHeader), &p_data, p_header.byteSize);
                        m_localHead += p_header.byteSize + p_header.bytePadding;

                        while(m_mutex.Lock(1000) == false)
                            Sleep(10);
                        memcpy(&m_fileMapInfoStructured, (unsigned char*)f_infoMapData, sizeof(FilemapInfo));
                        m_fileMapInfoStructured.head_ByteOffset = m_localHead;
                        memcpy((unsigned char*)f_infoMapData, &m_fileMapInfoStructured, sizeof(FilemapInfo));
                        m_mutex.Unlock();
                        PrintFilemapInfo(true);
                        return true;


                        break;
                }
                return false;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
            return true;
        }

        size_t Filemapping::MakeMultiple(size_t p_inputSize)
        {
            try
            {

                size_t modValue;
                modValue = p_inputSize % m_chunkSize;
                // std::cout << "\nModvalue: " << modValue << "\n";
                if(modValue == 0)
                {
                    return p_inputSize;
                }

                return p_inputSize + m_chunkSize - modValue;
            }
            catch(...)
            {
                const std::string errorMessage = std::string("Catch: " + std::string(__FUNCTION__));
                PrintError(MString() + errorMessage.c_str());
            }
        }
    }
}