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

            /** Initializes entity system and different engine DLLs for client*/
            void InitializeClient();

            /** Initializes entity system and different engine DLLs for server*/
            void InitializeServer();

            /**
                TODOCM doc
            */
            void UpdateGame(uint32_t p_deltaTime);

            /**
                TODOCM doc
            */
            void StartClientCore();

            /**
                TODOCM doc
            */
            void StartServerCore();

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