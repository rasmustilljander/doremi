#include <Internal/Logger/LoggerImpl.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>
#include <Utility/Utilities/Include/Logging/LogLevelConverter.hpp>
#include <Utility/Utilities/Include/Logging/LogTagConverter.hpp>
#include <Utility/Utilities/Include/Logging/HelpFunctions.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>

#include <Utility/Utilities/Include/String/VA_ListToString.hpp>
#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <Windows.h>

namespace DoremiEngine
{
    namespace Logging
    {
        using namespace Doremi::Utilities;
        using namespace Doremi::Utilities::Logging;

        void threadWork(bool* p_applicationOnline, Memory::CircleBuffer<LogTextData>* p_localBuffer, Memory::CircleBuffer<LogTextData>* m_outGoingBuffer);

        LoggerImpl::LoggerImpl()
            : m_localBuffer(nullptr), m_outGoingBuffer(nullptr), m_fileMap(nullptr), m_mutex(nullptr), m_applicationRunning(nullptr)
        {
            m_uniqueId = GetCurrentProcessId();
            Initialize();
        }

        LoggerImpl::~LoggerImpl()
        {
            if(m_localBuffer != nullptr)
            {
                delete m_localBuffer;
            }
            if(m_outGoingBuffer != nullptr)
            {
                delete m_outGoingBuffer;
            }

            if(m_fileMap != nullptr)
            {
                delete m_fileMap;
            }
            if(m_mutex != nullptr)
            {
                delete m_mutex;
            }
            if(m_applicationRunning != nullptr)
            {
                delete m_applicationRunning;
            }
        }

        void LoggerImpl::Initialize()
        {
            m_localBuffer = new Memory::CircleBuffer<LogTextData>();
            m_outGoingBuffer = new Memory::CircleBuffer<LogTextData>();

            // TODORT
            // TODOXX
            // TODOCONFIG our hardcode better value from empirical tests
            // Create localbuffer
            m_localBuffer->Initialize(10000, nullptr);

            // Create mutex
            m_mutex = CreateFileMapMutex();

            // Fetch shared memory from fileMap
            void* fileMapMemory = InitializeFileMap(Constants::IPC_FILEMAP_SIZE);

            // Intiailize outgoing buffer with shared memory and mutex
            m_outGoingBuffer->Initialize(fileMapMemory, Constants::IPC_FILEMAP_SIZE, m_mutex);
            m_applicationRunning = new bool(true);

            std::thread outGoingLoggingThread(threadWork, m_applicationRunning, m_localBuffer, m_outGoingBuffer);
            outGoingLoggingThread.detach();

#ifdef NO_LOGGER
#else
            StartLoggingProcess();
#endif
        }

        void LoggerImpl::LT(const std::string& p_function, const size_t& p_line, const LogTag& p_logTag, const LogLevel& p_logLevel, const char* p_format, ...)
        {
            // Build a string from va_list
            va_list args;
            va_start(args, p_format);
            std::string message;
            String::toString(message, p_format, args);
            va_end(args);

            Logging::LogTextData data;

            // Function
            const std::size_t functionLength = p_function.copy(data.function, std::max(p_function.size(), Constants::LONGEST_FUNCTION_NAME), 0);
            data.function[functionLength] = '\0';

            // Line
            data.line = p_line;

            // Message
            const std::size_t messageLength = message.copy(data.message, std::max(message.size(), Constants::LONGEST_FUNCTION_NAME), 0);
            data.message[messageLength] = '\0';

            // Level and tag
            data.logLevel = p_logLevel;
            data.logTag = p_logTag;

            using namespace Memory;
            Memory::CircleBufferHeader header;
            header.packageSize = sizeof(LogTextData);
            header.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
            bool succeed = false;


#ifdef NO_LOGGER
            m_localBuffer->Produce(header, &data);
#else
            while(!succeed) // TODORT Might not need while loop
            {
                succeed = m_localBuffer->Produce(header, &data);
            }
#endif
        }

        void* LoggerImpl::InitializeFileMap(const std::size_t& p_size)
        {
            m_fileMap = new IO::FileMap();
            void* memory = m_fileMap->Initialize(Logging::BuildFileMapName(m_uniqueId), p_size);
            if(memory != nullptr)
            {
                return memory;
            }
            else
            {
                throw std::runtime_error("Failed to initialize filemap.");
            }
        }

        IO::Mutex* LoggerImpl::CreateFileMapMutex()
        {
            IO::Mutex* mutex = new IO::FileMapMutex();
            const bool success = mutex->Initialize(Constants::IPC_FILEMAP_MUTEX_NAME);
            if(success)
            {
                return mutex;
            }
            else
            {
                delete mutex;
                throw std::runtime_error("Failed to initialize filemap mutex.");
            }
        }

        std::wstring LoggerImpl::BuildLoggingProcessArgumentString()
        {
            const std::wstring applicationPathW = String::s2ws(Constants::LOGGING_PROCESS_NAME).c_str();
            const std::wstring arguments = std::wstring(applicationPathW + std::wstring(L" ") + std::to_wstring(m_uniqueId));
            return arguments;
        }

        void LoggerImpl::StartLoggingProcess()
        {
            PROCESS_INFORMATION processInformation;
            STARTUPINFO startupInfo;
            ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
            ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
            startupInfo.cb = sizeof(STARTUPINFO);
            std::wstring& arguments = BuildLoggingProcessArgumentString();

            if(!CreateProcess(NULL, &arguments[0], 0, 0, 0, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, 0, 0, &startupInfo, &processInformation))
            {
                std::cout << GetLastError() << std::endl;
                throw std::runtime_error("Creating loggingprocess failed.");
            }
            CloseHandle(processInformation.hProcess);
            CloseHandle(processInformation.hThread);
        }

        void threadWork(bool* p_applicationOnline, Memory::CircleBuffer<LogTextData>* p_localBuffer, Memory::CircleBuffer<LogTextData>* m_outGoingBuffer)
        {
            bool messageExist = true;
            LogTextData* data = new LogTextData();
            Memory::CircleBufferHeader* header = new Memory::CircleBufferHeader();
            bool succeed = false;

            // As long as the application is running or if there are some messages ongoing
            while(*p_applicationOnline || messageExist) // TODORT, this might crash on crash....
            {
                using namespace std::literals;
                messageExist = p_localBuffer->Consume(header, data);
                if(messageExist)
                {

#ifdef NO_LOGGER
                    m_outGoingBuffer->Produce(*header, data);
#else
                    succeed = false;
                    while(!succeed)
                    {
                        succeed = m_outGoingBuffer->Produce(*header, data);
                    }
#endif
                }
                else
                {
                    // TODORT
                    // TODOXX Might not be long enough, or might be longer than required.
                    std::this_thread::sleep_for(Logging::Constants::LOGGING_PRODUCE_TIME_WAIT);
                }
            }
            delete data;
            delete header;
        }
    }
}
