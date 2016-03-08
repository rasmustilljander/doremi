#pragma once
#include <unordered_map>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        struct TimerData
        {
            explicit TimerData(uint32_t p_startLine) : data(0), startLine(p_startLine) {}

            ~TimerData() {}

            Utilities::Chrono::Timer timer;
            double data;
            uint32_t startLine;
        };

        struct TimerKey
        {
            TimerKey(const std::string& p_file, const std::string& p_function, const uint32_t& p_line)
                : file(p_file), function(p_function), line(p_line)
            {
            }

            ~TimerKey() {}

            bool operator==(const TimerKey& rhs) const { return (rhs.file == file && rhs.function == function && rhs.line == line); }

            std::string file;
            std::string function;
            uint32_t line;
        };

        struct TimerKeyHasher
        {
            std::size_t operator()(const TimerKey& key) const
            {
                std::hash<std::string> hasher;
                return hasher(key.file + key.function + std::to_string(key.line));
            }
        };

        class TimerManager
        {
        public:
            TimerManager(const TimerManager& p_timerManager) = delete;
            void operator=(const TimerManager&) = delete;

            /**
            TODO docs
            */
            static TimerManager& GetInstance();

            /**
            TODO docs
            */
            const TimerData* const StartTimer(const std::string& p_file, const std::string& p_function, const uint32_t& p_lineNumber);

            /**
            TODO docs
            */
            void TimerManager::StopTimer(const TimerData* const p_timer);

            /**
            TODO docs
            */
            void StartTimer(const std::string& p_name);

            /**
            TODO docs
            */
            void StopTimer(const std::string& p_name);


            /**
            TODO docs
            */
            void DumpData(const DoremiEngine::Core::SharedContext& p_sharedContext);

        private:
            TimerManager();
            virtual ~TimerManager();

            std::unordered_map<TimerKey, TimerData, TimerKeyHasher> m_timers;
            std::unordered_map<std::string, TimerData> m_namedTimers;
        };
    }
}