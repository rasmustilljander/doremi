#pragma once
#include <Logger/Logger.hpp>
#include <map>
#include <Utility/Utilities/Include/Memory/Circlebuffer/ArbitrarySizeCirclebuffer.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {
            class FileMap;
            class Mutex;
        }
        namespace Logging
        {
            enum class LogTag : uint8_t;
            enum class LogLevel : uint8_t;
        }
    }
}

namespace DoremiEngine
{
    namespace Logging
    {
        struct ThreadMetaData;

        class LoggerImpl : public Logger
        {
        public:
            /**
                Constructor
            */
            LoggerImpl();

            /**
            TODORT docs
            */
            void Initialize();

            /**
           Destuctor
            */
            virtual ~LoggerImpl();

            /**
            The actual method called when calling LogText
            */
            void LogTextReal(const std::string& p_function, const uint16_t& p_line, const Doremi::Utilities::Logging::LogTag& p_tag,
                             const Doremi::Utilities::Logging::LogLevel& p_vLevel, const char* p_format, ...) override;

        private:
            void* InitializeFileMap(const std::size_t& p_size);
            std::wstring BuildLoggingProcessArgumentString();
            void StartLoggingProcess();
            Doremi::Utilities::IO::Mutex* CreateFileMapMutex();

            Doremi::Utilities::Memory::ArbitrarySizeCirclebuffer* m_localBuffer;
            Doremi::Utilities::Memory::ArbitrarySizeCirclebuffer* m_outGoingBuffer;
            Doremi::Utilities::IO::FileMap* m_fileMap;
            Doremi::Utilities::IO::Mutex* m_mutex;
            bool* m_applicationRunning;
            ThreadMetaData* m_threadMetaData;
            int m_uniqueId;
        };
    }
}
