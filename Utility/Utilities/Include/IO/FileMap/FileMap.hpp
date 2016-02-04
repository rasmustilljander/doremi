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
                FileMap();

                /**
                TODO docs
                */
                virtual ~FileMap();

                /**
                Returns the adress of the filemap
                */
                void* Initialize(const std::string& p_name, const size_t& p_fileMapSize);

                /**
                TODO docs
                */
                void AttemptLock(const uint32_t& p_timeOut);


                /**
                TODO docs
                */
                void Lock();


                /**
                TODO docs
                */
                void UnLock();

            protected:
                bool OpenFileMap();

                bool MapFileMapIntoMemory();

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
