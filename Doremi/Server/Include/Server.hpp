#pragma once
#include <Doremi/Core/Include/GameCore.hpp>
#include <vector>
#include <map>
#include <Windows.h>

namespace Doremi
{
    namespace Core
    {
        class Manager;
    }
}

namespace Doremi
{

    class ServerMain : public Core::GameCore
    {
    public:
        /**
            Constructor
        */
        ServerMain();

        /**
            Destructor
        */
        virtual ~ServerMain();

        /**
            doc
        */
        void Start();

        /**
        doc
        */
        void Stop() override;

    private:
        /**
            TODOCM doc
        */
        void Initialize();

        /**
            TODOCM doc
        */
        void SpawnDebugWorld(const DoremiEngine::Core::SharedContext& sharedContext);

        /**
            TODOCM doc
        */
        void Run();

        void TrackMemoryLeak(std::string p_name, bool shouldStart);

        /**
            TODOCM doc
        */
        void UpdateGame(double p_deltaTime);

        /**
            TOODCM doc
        */
        std::vector<Core::Manager*> m_managers;
        // Track memory leak
        std::map<std::string, SSIZE_T> m_memoryLeakFromStringDelta;
        std::map<std::string, SSIZE_T> m_memoryLeakFromString;

        SSIZE_T m_lastMemory;
    };
}