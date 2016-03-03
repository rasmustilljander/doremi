#pragma once

// Standard libraries
#include <functional>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Logging
    {
        class Logger;
    }
}

namespace Doremi
{
    namespace Core
    {
        class GameCore
        {
        public:
            /**
            TODO doc
            */
            GameCore();
            /**
            TODO doc
            */
            virtual ~GameCore();

        protected:
            void LoadEngineLibrary();
            const DoremiEngine::Core::SharedContext& InitializeEngine(const size_t& p_engineModulesToStart);

            // Engine variables
            const DoremiEngine::Core::SharedContext* m_sharedContext;
            std::function<void()> m_stopEngineFunction;
            void* m_engineLibrary;
            DoremiEngine::Logging::Logger* m_logger;
        };
    }
}