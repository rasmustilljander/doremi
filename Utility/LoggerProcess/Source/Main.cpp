#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/Circlebuffer.hpp>
#include <Utility/Utilities/Include/Constants/LoggerConstants.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>

#include <windows.h>
#include <time.h>
#include <string>

#include <iostream>
#include <fstream>

using namespace Doremi::Utilities;

std::string getTwoDigit(const int& value)
{
    using namespace std;
    if(value < 10)
    {
        return string("0" + to_string(value));
    }

    return to_string(value);
}

std::string buildLogFileName(const std::string& p_consoleName)
{
    using namespace std;
    struct tm now;
    time_t time_create = time(NULL);
    localtime_s(&now, &time_create);

    string fileName = string("logs/");
    fileName.append(getTwoDigit(now.tm_year + 1900) + "-");
    fileName.append(getTwoDigit(now.tm_mon + 1) + "-");
    fileName.append(getTwoDigit(now.tm_mday) + "_");
    fileName.append(getTwoDigit(now.tm_hour) + ".");
    fileName.append(getTwoDigit(now.tm_min));
    fileName.append("__" + p_consoleName);
    fileName.append(".txt");
    return fileName;
}

class LoggerProcess
{
public:
    LoggerProcess() : m_fileMap(nullptr), m_ingoingBuffer(nullptr), m_mutex(nullptr) {}

    virtual ~LoggerProcess() {}


    void Initialize()
    {
        SetupCircleBuffer();
        void* fileMapMemory = InitializeFileMap(Constants::FILEMAP_SIZE);
        m_mutex = CreateFileMapMutex();

        m_ingoingBuffer->Initialize(fileMapMemory, Constants::FILEMAP_SIZE, m_mutex);
    }

    void Run()
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
                std::cout << data->message << std::endl;
                elapsedTime = 0;
            }

            elapsedTime += m_timer.GetElapsedTimeInSeconds();
            if(elapsedTime > Constants::FILEMAP_TIMEOUT)
            {
                break;
            }
        }
        delete data;
        delete header;
    }

private:
    Chrono::Timer m_timer;
    IO::FileMap* m_fileMap;
    Memory::CircleBuffer<Logging::LogTextData>* m_ingoingBuffer;
    Doremi::Utilities::IO::Mutex* m_mutex;

    void* InitializeFileMap(const std::size_t& p_size)
    {
        m_fileMap = new IO::FileMap();
        void* memory = m_fileMap->Initialize(Constants::FILEMAP_NAME, p_size);
        if(memory != nullptr)
        {
            return memory;
        }
        else
        {
            throw std::runtime_error("Failed to initialize filemap.");
        }
    }

    void SetupCircleBuffer() { m_ingoingBuffer = new Memory::CircleBuffer<Logging::LogTextData>(); }

    IO::Mutex* CreateFileMapMutex()
    {
        IO::Mutex* mutex = new IO::FileMapMutex();
        const bool success = mutex->Initialize(Constants::FILEMAP_MUTEX_NAME);
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
};

int main(int argc, char** argv)
{
    using namespace Doremi::Utilities;
    using namespace std;

    HANDLE fileHandle;

    // Build logfilename
    // const string& fileName = buildLogFileName("Doremi");

    // Create the file
    // CreateDirectory(L"logs", NULL);
    // fileHandle = CreateFile(String::s2ws(fileName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);

    //	SetConsoleTitle(s2ws(consoleName).c_str());
    //	SetConsoleTextAttribute(out, color);
    LoggerProcess application;
    application.Initialize();
    application.Run();

    return 0;
}