#pragma once

#include <Windows.h>
#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMapMutex.hpp>
#include <cstdint>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {

            class FileMap
            {
            public:
                /**
                TODO docs
                */
                FileMap() : m_mapHandle(nullptr), m_fileMapSize(0), m_rawMemoryOfMappedFile(nullptr) {}

                /**
                TODO docs
                */
                virtual ~FileMap() { CloseHandle(m_mapHandle); }

                /**
                TODO docs
                */
                void* Initialize(const std::string& p_name, const size_t& p_fileMapSize)
                {
                    m_name = p_name;
                    m_fileMapSize = p_fileMapSize;
                    bool success = false;

                    success = OpenFileMap();
                    if(!success)
                    {
                        // TODORT LOG
                        // TODOLOG
                        return nullptr;
                    }
                    success = MapFileMapIntoMemory();
                    if(!success)
                    {
                        // TODORT LOG
                        // TODOLOG
                        return nullptr;
                    }
                    return m_rawMemoryOfMappedFile;
                }

                bool OpenFileMap()
                {
                    m_mapHandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, (DWORD)0, m_fileMapSize, String::s2ws(m_name).c_str());
                    if(m_mapHandle != NULL)
                    {
                        return true;
                    }
                    return false;
                }

                bool MapFileMapIntoMemory()
                {
                    m_rawMemoryOfMappedFile = MapViewOfFile(m_mapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
                    if(m_rawMemoryOfMappedFile != NULL)
                    {
                        return true;
                    }
                    return false;
                }

                void AttemptLock(const uint32_t& p_timeOut) { m_mutex.AttemptLock(p_timeOut); }

                void Lock() { m_mutex.Lock(); }

                void UnLock() {}

            private:
                HANDLE m_mapHandle;
                std::string m_name;
                size_t m_fileMapSize;
                void* m_rawMemoryOfMappedFile;
                FileMapMutex m_mutex;
            };
        }
    }
}
