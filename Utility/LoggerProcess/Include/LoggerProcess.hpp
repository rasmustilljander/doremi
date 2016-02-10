#pragma once

#include <SpecificLogFile.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>

#include <Utility/Utilities/Include/Memory/Circlebuffer/Circlebuffer.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <map>

namespace Doremi
{
    namespace Utilities
    {
        namespace IO
        {
            class FileMap;
            class Mutex;
        }
    }
}

class LoggerProcess
{
public:
    /**
    TODO docs
    */
    LoggerProcess();

    /**
    TODO docs
    */
    virtual ~LoggerProcess();

    /**
    TODO docs
    */
    void Initialize(const int& p_uniqueId);

    /**
    TODO docs
    */
    void Run();

private:
    void* InitializeFileMap(const std::size_t& p_size, const int& p_uniqueId);
    void SetupCircleBuffer();
    Doremi::Utilities::IO::Mutex* CreateFileMapMutex();
    void SetupFolderStructure();
    void BuildLogFiles();

    Doremi::Utilities::Chrono::Timer m_timer;
    Doremi::Utilities::IO::FileMap* m_fileMap;
    Doremi::Utilities::Memory::CircleBuffer<Doremi::Utilities::Logging::LogTextData>* m_ingoingBuffer;
    Doremi::Utilities::IO::Mutex* m_mutex;
    std::map<Doremi::Utilities::Logging::LogTag, SpecificLogFile> m_logfiles;
};