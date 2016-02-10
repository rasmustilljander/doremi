#include <LoggerProcess.hpp>

#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <Utility/Utilities/Include/Constants/LoggerConstants.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <Utility/Utilities/Include/Logging/HelpFunctions.hpp>

#include <time.h>
#include <Windows.h>
#include <iostream>

using namespace Doremi::Utilities;

LoggerProcess::LoggerProcess() : m_fileMap(nullptr), m_ingoingBuffer(nullptr), m_mutex(nullptr)
{
    // TODORT
    // TODOXX
}

LoggerProcess::~LoggerProcess() {}

void LoggerProcess::Initialize(const int& p_uniqueId)
{
    SetupFolderStructure();
    BuildLogFiles();
    SetupCircleBuffer();
    void* fileMapMemory = InitializeFileMap(Constants::IPC_FILEMAP_SIZE, p_uniqueId);
    m_mutex = CreateFileMapMutex();

    m_ingoingBuffer->Initialize(fileMapMemory, Constants::IPC_FILEMAP_SIZE, m_mutex);
}

void LoggerProcess::Run()
{
    using namespace Doremi::Utilities;

    Logging::LogTextData* data = new Logging::LogTextData();
    Memory::CircleBufferHeader* header = new Memory::CircleBufferHeader();
    bool messageExist = false;
    double elapsedTime = 0;
    while(true)
    {
        m_timer.Tick();
        messageExist = m_ingoingBuffer->Consume(header, data);
        if(messageExist)
        {
            m_logfiles[data->logTag].Write(*data);
            std::cout << data->message << "\n";
            elapsedTime = 0;
        }

        elapsedTime += m_timer.GetElapsedTimeInSeconds();
        if(elapsedTime > Constants::IPC_FILEMAP_TIMEOUT)
        {
            break;
        }
    }
    delete data;
    delete header;
}

void* LoggerProcess::InitializeFileMap(const std::size_t& p_size, const int& p_uniqueId)
{
    m_fileMap = new IO::FileMap();
    std::string fileMapName;

    // If id is zero, default, otherwise build uniquename.
    if(p_uniqueId == 0)
    {
        fileMapName = Constants::IPC_DEFAULT_FILEMAP_NAME;
    }
    else
    {
        fileMapName = Logging::BuildFileMapName(p_uniqueId);
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
    const string& folderName = Logging::BuildFolderNameBasedOnTime();

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
        m_logfiles[tag] = SpecificLogFile();
        m_logfiles[tag].Initialize(tag);
    }
}