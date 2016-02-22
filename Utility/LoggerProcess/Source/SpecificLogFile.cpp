#include <SpecificLogFile.hpp>

#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <Utility/Utilities/Include/Logging/LogTagConverter.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>
#include <Utility/Utilities/Include/String/StringHelper.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <Utility/Utilities/Include/PointerArithmetic/PointerArithmetic.hpp>

#include <iostream>
#include <exception>
#include <Windows.h>

using namespace Doremi::Utilities;

SpecificLogFile::SpecificLogFile() : m_fileStream(nullptr), m_timer(nullptr), m_flushTimerLimit(0), m_elapsedTime(0) {}

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
    m_flushTimerLimit = 1.0;
    m_timer = new Chrono::Timer();
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

void SpecificLogFile::Write(void*& p_data)
{
    // Rebuilddata
    const Logging::TextMetaData* textMetaData = static_cast<Logging::TextMetaData*>(p_data);

    // Fetch pointer to function text
    void* function = PointerArithmetic::Addition(p_data, sizeof(Logging::TextMetaData));

    // Fetch pointer to messagetext
    void* message = PointerArithmetic::Addition(p_data, sizeof(Logging::TextMetaData) + textMetaData->functionLength);

    // Actually cout the data to a file
    *m_fileStream << static_cast<char*>(message) << "\n";
    if(textMetaData->logLevel == Logging::LogLevel::INFO)
    {
        std::cout << static_cast<char*>(message) << "\n";
    }

    // If called often, flush
    m_elapsedTime += m_timer->Tick().GetElapsedTimeInSeconds();
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
