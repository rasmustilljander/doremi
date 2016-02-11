#include <SpecificLogFile.hpp>

#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <Utility/Utilities/Include/Logging/LogTagConverter.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>
#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>

#include <iostream>
#include <exception>
#include <Windows.h>

using namespace Doremi::Utilities;

SpecificLogFile::SpecificLogFile() : m_fileStream(nullptr), m_timer(nullptr), m_flushTimerLimit(0), m_elapsedTime(0)
{
    m_timer = new Chrono::Timer();

    // TODORT
    // TODOXX Might decrease / increase the performance
    // TODOCONF ??
    m_flushTimerLimit = 1.0;
}

SpecificLogFile::~SpecificLogFile()
{
    if(m_fileStream != nullptr)
    {
        m_fileStream->close();
        delete m_fileStream;
    }

    if(m_timer != nullptr)
    {
        delete m_timer;
    }
}

void SpecificLogFile::Initialize(const Logging::LogTag& p_logTag)
{
    Logging::LogTagInfo fileNameInfo = Logging::LogTagConverter::convert(p_logTag);
    fileNameInfo.name.append(".txt");
    BuildLogFile(fileNameInfo.name);
    OpenFileStream(fileNameInfo.name);
}

void SpecificLogFile::BuildLogFile(const std::string& p_fileName)
{
    void* fileHandle = CreateFile(String::StringToWideString(p_fileName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
    if(fileHandle == nullptr)
    {
        const std::string message = std::string("Failed to build logfile with given name: ") + p_fileName;
        throw std::runtime_error(message);
    }
    CloseHandle(fileHandle);
}

void SpecificLogFile::OpenFileStream(const std::string& p_fileName)
{
    m_fileStream = new std::ofstream(p_fileName, std::ofstream::out | std::ofstream::app);
    if(m_fileStream->is_open() == false)
    {
        const std::string message = std::string("Failed to open filestream to given filename: ") + p_fileName;
        throw std::runtime_error(message);
    }
}

void SpecificLogFile::Write(const Doremi::Utilities::Logging::LogTextData& p_data)
{
    *m_fileStream << p_data.message << "\n";
    m_elapsedTime += m_timer->Tick().GetElapsedTimeInSeconds();

    // If called often, flush
    if(m_elapsedTime > m_flushTimerLimit)
    {
        Flush();
    }
}

void SpecificLogFile::Flush()
{
    m_elapsedTime = 0;
    m_fileStream->flush();
}
