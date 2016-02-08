#include <IO/FileMap/FileMap.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {
            FileMap::FileMap() : m_mapHandle(nullptr), m_fileMapSize(0), m_rawMemoryOfMappedFile(nullptr) {}

            FileMap::~FileMap()
            {
                if(m_mapHandle != nullptr)
                {
                    CloseHandle(m_mapHandle);
                }
            }

            void* FileMap::Initialize(const std::string& p_name, const size_t& p_fileMapSize)
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

            void FileMap::AttemptLock(const uint32_t& p_timeOut) { m_mutex.AttemptLock(p_timeOut); }

            void FileMap::Lock() { m_mutex.Lock(); }

            void FileMap::UnLock() { m_mutex.Unlock(); }

            bool FileMap::OpenFileMap()
            {
                m_mapHandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, (DWORD)0, m_fileMapSize, String::s2ws(m_name).c_str());
                if(m_mapHandle != NULL)
                {
                    return true;
                }
                return false;
            }

            bool FileMap::MapFileMapIntoMemory()
            {
                m_rawMemoryOfMappedFile = MapViewOfFile(m_mapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
                if(m_rawMemoryOfMappedFile != NULL)
                {
                    return true;
                }
                return false;
            }
        }
    }
}