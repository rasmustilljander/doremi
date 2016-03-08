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
#include <Utility/Utilities/Include/PointerArithmetic/PointerArithmetic.hpp>

#include <iostream>
#include <algorithm>
#include <chrono>
#include <Windows.h>

namespace DoremiEngine
{
    namespace Logging
    {
        // TODORT, move to cpp
        struct ThreadMetaData
        {
            ThreadMetaData() : isApplicationOnline(nullptr), isThreadStillRunning(nullptr) {}

            ThreadMetaData(bool* p_isApplicationOnline, bool p_isThreadStillRunning)
                : isApplicationOnline(p_isApplicationOnline), isThreadStillRunning(new bool(p_isThreadStillRunning))
            {
            }

            ~ThreadMetaData()
            {
                if(isThreadStillRunning != nullptr)
                {
                    delete isThreadStillRunning;
                }
            }
            ThreadMetaData(const ThreadMetaData& threadMetaData) = delete;

            const bool* isApplicationOnline;
            bool* isThreadStillRunning;
        };

        using namespace Doremi::Utilities;
        using namespace Doremi::Utilities::Logging;

        void ThreadWork(ThreadMetaData* p_threadMetaData, Memory::ArbitrarySizeCirclebuffer* p_localBuffer, Memory::ArbitrarySizeCirclebuffer* m_outGoingBuffer);

        LoggerImpl::LoggerImpl()
            : m_localBuffer(nullptr), m_outGoingBuffer(nullptr), m_fileMap(nullptr), m_mutex(nullptr), m_applicationRunning(nullptr), m_threadMetaData(nullptr)
        {
#ifdef NO_LOGGER
            return;
#endif
            m_uniqueId = GetCurrentProcessId();
            Initialize();
        }

        LoggerImpl::~LoggerImpl()
        {
#ifdef NO_LOGGER
            return;
#endif
            try
            {
                if(m_threadMetaData != nullptr)
                {
                    *m_applicationRunning = false;
                    while(*m_threadMetaData->isThreadStillRunning == true)
                    {
                        using namespace std::literals;
                        std::this_thread::sleep_for(1ms);
                    }
                }
            }
            catch(...)
            {
                std::cout << "Failed to shutdown threads properly" << std::endl;
            }

            if(m_fileMap != nullptr)
            {
                delete m_fileMap;
            }

            if(m_localBuffer != nullptr)
            {
                delete m_localBuffer;
            }
            if(m_outGoingBuffer != nullptr)
            {
                delete m_outGoingBuffer;
            }
            if(m_mutex != nullptr)
            {
                delete m_mutex;
            }
            if(m_threadMetaData != nullptr)
            {
                delete m_threadMetaData;
            }
            if(m_applicationRunning != nullptr)
            {
                delete m_applicationRunning;
            }
        }

        void LoggerImpl::Initialize()
        {
#ifdef NO_LOGGER
            return;
#endif
            m_localBuffer = new Memory::ArbitrarySizeCirclebuffer();
            m_outGoingBuffer = new Memory::ArbitrarySizeCirclebuffer();

            // TODOXX TODORT our hardcode better value from empirical tests
            // Create localbuffer
            m_localBuffer->Initialize(10000);

            // Create mutex
            m_mutex = CreateFileMapMutex();

            // Fetch shared memory from fileMap
            void* fileMapMemory = InitializeFileMap(Constants::IPC_FILEMAP_SIZE);

            // Intiailize outgoing buffer with shared memory and mutex
            m_outGoingBuffer->Initialize(fileMapMemory, Constants::IPC_FILEMAP_SIZE, m_mutex);
            m_applicationRunning = new bool(true);

            m_threadMetaData = new ThreadMetaData(m_applicationRunning, new bool(true));
            std::thread outGoingLoggingThread(ThreadWork, m_threadMetaData, m_localBuffer, m_outGoingBuffer);
            outGoingLoggingThread.detach();
            StartLoggingProcess();
        }

        void LoggerImpl::LogTextReal(const std::string& p_function, const uint16_t& p_line, const LogTag& p_logTag, const LogLevel& p_logLevel,
                                     const char* p_format, ...)
        {
            using namespace Doremi::Utilities::Logging;
            using namespace Doremi::Utilities;
            // Build a string from va_list
            va_list args;
            va_start(args, p_format);
            if(p_logLevel != LogLevel::MASS_DATA_PRINT)
            {
                printf("[%s:%s] ", LogTagConverter::convert(p_logTag).name.c_str(), LogLevelConverter::convert(p_logLevel).name.c_str());
                vprintf(p_format, args);
                printf("\n");
            }
#ifdef NO_LOGGER
            va_end(args);
            return;
#endif
            std::string message;
            String::toString(message, p_format, args);
            va_end(args);

            // Allocate buffer
            const uint16_t functionSize = p_function.size() + 1; // The plus one comes as the null terminator
            const uint16_t messageSize = message.size() + 1; // The plus one comes as the null terminator
            const uint32_t bufferSize = functionSize + messageSize + sizeof(TextMetaData);

            void* buffer = malloc(bufferSize); // TODORT only use one buffer and lock this buffer
            memset(buffer, 0, bufferSize); // Do not remove this, used to get null terminator in the strings
            char* charBuffer = static_cast<char*>(buffer);

            // Write textmetadata
            *static_cast<TextMetaData*>(buffer) = TextMetaData(p_logTag, p_logLevel, p_line, functionSize, messageSize);
            charBuffer = PointerArithmetic::Addition(charBuffer, sizeof(TextMetaData));

            // Function
            const std::size_t functionLength = p_function.copy(charBuffer, functionSize, 0); // Copy functionString to buffer
            charBuffer = PointerArithmetic::Addition(charBuffer, functionLength + 1); // The plus one comes as the null terminator

            // Message
            const std::size_t messageLength = message.copy(charBuffer, messageSize, 0); // Copy messageString to buffer
            charBuffer = PointerArithmetic::Addition(charBuffer, messageLength + 1); // The plus one comes as the null terminator

            using namespace Memory;
            Memory::CircleBufferHeader header;
            header.packageSize = bufferSize;
            header.packageType = CircleBufferType(CircleBufferTypeEnum::TEXT);
            bool succeed = false;

            while(!succeed)
            {
                try
                {
                    m_localBuffer->Produce(header, buffer);
                    succeed = true;
                }
                catch(...)
                {
                    // Do nothing
                    // TODOXX TODORT NOt very performance nice
                    std::this_thread::sleep_for(Logging::Constants::LOGGING_PRODUCE_TIME_WAIT);
                }
            }
            free(buffer);
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
            const bool success = mutex->Initialize(Logging::BuildFileMapMutexName(m_uniqueId));
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

        void ThreadWork(ThreadMetaData* p_threadMetaData, Memory::ArbitrarySizeCirclebuffer* p_localBuffer, Memory::ArbitrarySizeCirclebuffer* m_outGoingBuffer)
        {
            try
            {
                bool messageExist = true;
                const uint32_t sizeofBuffer = 1000;
                void* buffer = malloc(sizeofBuffer); // TODORT TODOXX Might need to find a better value.
                Memory::CircleBufferHeader* header = new Memory::CircleBufferHeader();
                header->packageSize = sizeofBuffer; // TODORT TODOXX Might need to find a better value.
                bool succeed = false;

                // As long as the application is running or if there are some messages ongoing
                while(*p_threadMetaData->isApplicationOnline || messageExist)
                {
                    using namespace std::literals;
                    messageExist = p_localBuffer->Consume(header, buffer, sizeofBuffer);
                    if(messageExist)
                    {
                        succeed = false;
                        while(!succeed)
                        {
                            succeed = m_outGoingBuffer->Produce(*header, buffer);
                            if(!succeed)
                            {
                                std::this_thread::sleep_for(Logging::Constants::LOGGING_PRODUCE_TIME_WAIT);
                            }
                        }
                    }
                    else
                    {
                        // TODORT
                        // TODOXX Might not be long enough, or might be longer than required.
                        std::this_thread::sleep_for(Logging::Constants::LOGGING_PRODUCE_TIME_WAIT);
                    }
                }

                free(buffer);
                delete header;
                *p_threadMetaData->isThreadStillRunning = false;
            }
            catch(...)
            {
                *p_threadMetaData->isThreadStillRunning = false;
            }
        }
    }
}
