#include <LoggerProcess.hpp>

#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <Utility/Utilities/Include/Constants/LoggerConstants.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <Utility/Utilities/Include/Logging/HelpFunctions.hpp>

#include <time.h>
#include <Windows.h>
#include <iostream>
#include <thread>

using namespace Doremi::Utilities;

LoggerProcess::LoggerProcess() : m_fileMap(nullptr), m_ingoingBuffer(nullptr), m_mutex(nullptr) {}

LoggerProcess::~LoggerProcess()
{
    if(m_fileMap != nullptr)
    {
        delete m_fileMap;
    }

    if(m_ingoingBuffer != nullptr)
    {
        delete m_ingoingBuffer;
    }

    if(m_mutex != nullptr)
    {
        delete m_mutex;
    }
}

void LoggerProcess::Initialize(const int& p_uniqueId)
{
    m_processIdOfGame = p_uniqueId;
    SetupFolderStructure();
    BuildLogFiles();
    SetupCircleBuffer();
    void* fileMapMemory = InitializeFileMap(Constants::IPC_FILEMAP_SIZE);
    m_mutex = CreateFileMapMutex();

    m_ingoingBuffer->Initialize(fileMapMemory, Constants::IPC_FILEMAP_SIZE, m_mutex);
}

void LoggerProcess::Run()
{
    using namespace Doremi::Utilities;

    // Create temporary data/header memory
    Logging::LogTextData* data = new Logging::LogTextData();
    Memory::CircleBufferHeader* header = new Memory::CircleBufferHeader();

    bool messageExist = false;
    bool gameIsRunning = true;
    double elapsedTimeSinceLastEntry = 0;
    double flushTimer = 0;
    m_timer.Tick();
    while(gameIsRunning || messageExist)
    {
        // Consume data from shared memory
        messageExist = m_ingoingBuffer->Consume(header, data);
        gameIsRunning = IsGameRunning();

        // If any data existed
        if(messageExist)
        {
            // Send data to logfile
            m_logfiles[data->logTag].Write(*data);

            // Print data to console
            // std::cout << data->message << "\n";

            // Reset elapsed time
            elapsedTimeSinceLastEntry = 0;
        }
        else
        {
            using namespace std::literals;
            // TODORT
            // TODOXX is 10ms good?
            std::this_thread::sleep_for(10ms);
        }

        // Compute delta time
        m_timer.Tick();
        const double deltaTime = m_timer.GetElapsedTimeInSeconds();

        // Check if time for flush
        flushTimer += deltaTime;
        if(flushTimer > Constants::LOGFILE_FLUSH_INTERVAL)
        {
            for(auto& logfile : m_logfiles)
            {
                logfile.second.Flush();
            }
        }

        // If elapsed time since last log is greater than a timeout
        elapsedTimeSinceLastEntry += deltaTime;
        if(elapsedTimeSinceLastEntry > Constants::IPC_FILEMAP_TIMEOUT)
        {
            // Shutdown
            // break; //TODORT Not sure that the loggerprocess must have a hard timeout
        }
    }

    // Release temporary memory
    delete data;
    delete header;
}

void* LoggerProcess::InitializeFileMap(const std::size_t& p_size)
{
    m_fileMap = new IO::FileMap();
    std::string fileMapName;

    // If id is zero, default, otherwise build uniquename.
    if(m_processIdOfGame == 0)
    {
        fileMapName = Constants::IPC_DEFAULT_FILEMAP_NAME;
    }
    else
    {
        fileMapName = Logging::BuildFileMapName(m_processIdOfGame);
    }

    void* memory = m_fileMap->Initialize(fileMapName, p_size);
    if(memory != nullptr)
    {
        return memory;
    }
    else
    {
        throw std::runtime_error("Failed to initialize filemap.");
    }
}

void LoggerProcess::SetupCircleBuffer() { m_ingoingBuffer = new Memory::CircleBuffer<Logging::LogTextData>(); }

IO::Mutex* LoggerProcess::CreateFileMapMutex()
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

void LoggerProcess::SetupFolderStructure()
{
    using namespace std;
    // Build logfilename
    string& folderName = Logging::BuildFolderNameBasedOnTime();
    folderName.append("_" + std::to_string(GetCurrentProcessId()));

    // Create the directory
    CreateDirectory(L"logs", NULL);
    CreateDirectory(String::StringToWideString(folderName).c_str(), NULL);

    // Set working directory
    SetCurrentDirectory(String::StringToWideString(folderName).c_str());
}

void LoggerProcess::BuildLogFiles()
{
    for(auto& tag : Logging::ALL_LOG_TAGS)
    {
        m_logfiles.emplace(tag, SpecificLogFile());
        m_logfiles[tag].Initialize(tag);
    }
}

bool LoggerProcess::IsGameRunning()
{
    bool returnValue = false;

    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_processIdOfGame);
    if(processHandle != NULL)
    {
        returnValue = true;
    }

    CloseHandle(processHandle);

    return returnValue;
}
