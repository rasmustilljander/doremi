#pragma once
// Project specific

// Standard libraries
#include <vector>
#include <functional>
namespace Doremi
{
    namespace Core
    {
        class Manager;
        class EntityInterface;
        class GameCore
        {
            public:
            GameCore();
            virtual ~GameCore();
            /** Initializes entity system and different engine DLLs*/
            void Initialize();
            /** Starts the game. Game loop*/
            void StartCore();

            private:
            void LoadEngineLibrary();

            // Engine variables
            std::function<void()> m_stopEngineFunction;
            void* m_engineLibrary;

            // Managers
            std::vector<Manager*> m_managers;
        };
    }
}