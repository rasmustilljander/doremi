#pragma once
#include <Doremi/Core/Include/GameCore.hpp>
#include <vector>
#include <Doremi/Core/Include/Helper/MenuStates.hpp>

namespace Doremi
{
    namespace Core
    {
        class Manager;
        class EntityInterface;
    }
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
        void Update(double p_deltaTime);

        /**
            A method to send the draw to the correct path using the menustate
        */
        void Draw(double p_deltaTime);

        /**
        TODOCM doc
        */
        void UpdateGame(double p_deltaTime);

        /**
        TODOCM doc
        */
        void UpdateMenu(double p_deltaTime);
        /**
            TODOCM doc
        */
        void DrawGame(double p_deltaTime);

        /**
            Draws the menu screen
        */
        void DrawMenu(double p_deltaTime);
        /**
            TOODCM doc
        */
        std::vector<Core::Manager*> m_managers;

        /**
            TODOCM doc
        */
        std::vector<Core::Manager*> m_graphicalManagers;

        private:
        Doremi::MenuStates::MenuState m_menuState;
    };
}