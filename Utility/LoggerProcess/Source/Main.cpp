#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <Utility/Utilities/Include/IO/FileMap/FileMap.hpp>
#include <Utility/Utilities/Include/Memory/Circlebuffer/Circlebuffer.hpp>
#include <Utility/Utilities/Include/Constants/LoggerConstants.hpp>

#include <windows.h>
#include <time.h>
#include <string>

#include <iostream>
#include <fstream>

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
    LoggerProcess() {}

    virtual ~LoggerProcess() {}

    void Initialize() {}

    void Run()
    {
        using namespace Doremi::Utilities;

        while(true)
        {
            if(true)
            {
                m_timer.Tick();
                //  if (m_timer.GetElapsedTimeInSeconds() > Constants::FILEMAP_TIMEOUT)
                {
                    //    break;
                }
            }
        }
    }

private:
    Doremi::Utilities::Chrono::Timer m_timer;
    Doremi::Utilities::IO::FileMap m_fileMap;
    Doremi::Utilities::Memory::CircleBuffer<int> m_circleBuffer; // todo, real package data

    void* InitializeFileMap()
    {
        using namespace Doremi::Utilities;
        // m_fileMap = IO::FileMap(); // TODORT
        return m_fileMap.Initialize(Constants::FILEMAP_NAME, Constants::FILEMAP_SIZE);
    }

    void SetupCircleBuffer()
    {
        // m_circleBuffer.
    }
};

int main(int argc, char** argv)
{
    return 0;
    using namespace Doremi::Utilities;
    using namespace std;

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE fileHandle;

    // Build logfilename
    const string& fileName = buildLogFileName("Doremi");

    // Create the file
    CreateDirectory(L"logs", NULL);
    fileHandle = CreateFile(String::s2ws(fileName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);

    //	SetConsoleTitle(s2ws(consoleName).c_str());
    //	SetConsoleTextAttribute(out, color);
    LoggerProcess application;
    application.Run();

    return 0;
}