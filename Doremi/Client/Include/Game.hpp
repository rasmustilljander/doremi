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
    class GameMain : public Core::GameCore
    {
        public:
        /**
            Constructor
        */
        GameMain();

        /**
            Destructor
        */
        virtual ~GameMain();

        /**
            TODOCM doc
        */
        void Start();

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
        void UpdatetGame(double p_deltaTime);
        /**
            TODOCM doc
        */
        void DrawGame(double p_deltaTime);

        /**
            TOODCM doc
        */
        std::vector<Core::Manager*> m_managers;

        /**
            TODOCM doc
        */
        std::vector<Core::Manager*> m_graphicalManagers;
    };
}