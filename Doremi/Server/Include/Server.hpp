#pragma once
#include <Doremi/Core/Include/GameCore.hpp>
#include <vector>

namespace Doremi
{
    namespace Core
    {
        class Manager;
        class EntityInterface;
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
        void Stop();

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

        /**
            TODOCM doc
        */
        void UpdateGame(double p_deltaTime);

        /**
            TOODCM doc
        */
        std::vector<Core::Manager*> m_managers;
    };
}