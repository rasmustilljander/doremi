#pragma once

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

            protected:
                bool OpenFileMap();

                bool MapFileMapIntoMemory();

            private:
                void* m_mapHandle;
                std::string m_name;
                size_t m_fileMapSize;
                void* m_rawMemoryOfMappedFile;
            };
        }
    }
}
