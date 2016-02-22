#pragma once
#include <Logger/Logger.hpp>
#include <map>
#include <Utility/Utilities/Include/Memory/Circlebuffer/Circlebuffer.hpp>
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
            enum class LogTag;
            enum class LogLevel;
        }
    }
}

namespace DoremiEngine
{
    namespace Logging
    {

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
            void DebugLogReal(const std::string& p_function, const size_t& p_line, const Doremi::Utilities::Logging::LogTag& p_tag,
                              const Doremi::Utilities::Logging::LogLevel& p_vLevel, const char* p_format, ...) override;

        private:
            void* InitializeFileMap(const std::size_t& p_size);
            std::wstring BuildLoggingProcessArgumentString();
            void StartLoggingProcess();

            Doremi::Utilities::Memory::CircleBuffer<Doremi::Utilities::Logging::LogTextData>* m_localBuffer;
            Doremi::Utilities::Memory::CircleBuffer<Doremi::Utilities::Logging::LogTextData>* m_outGoingBuffer;
            Doremi::Utilities::IO::FileMap* m_fileMap;
            Doremi::Utilities::IO::Mutex* m_mutex;
            Doremi::Utilities::IO::Mutex* CreateFileMapMutex();
            bool* m_applicationRunning;
            int m_uniqueId;
        };
    }
}
