#pragma once
// Project specific

// Standard libraries
#include <vector>
#include <functional>

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
            void StartClientCore();

            /**
                TODOCM doc
            */
            void StartServerCore();

            private:
            /**
                TODOCM doc
            */
            void UpdateServerGame(double p_deltaTime);

            /**
                TODOCM doc
            */
            void UpdateClientGame(double p_deltaTime);

            /**
                TODOCM doc
            */
            void DrawGame(double p_deltaTime);


            /**
                TODO doc
            */
            void LoadEngineLibrary();

            // Engine variables
            std::function<void()> m_stopEngineFunction;
            void* m_engineLibrary;

            // Managers
            std::vector<Manager*> m_managers;
            std::vector<Manager*> m_graphicalManagers;

            protected:
            /**
                TODOCM doc
            */
            const DoremiEngine::Core::SharedContext& InitializeEngine(const size_t& p_engineModulesToStart);

            /**
                TOODCM test remove
            */
            void GenerateWorldServerJawsTest(const DoremiEngine::Core::SharedContext& sharedContext);

            /**
                TODOCM test remove
            */
            void GenerateWorldClientJawsTest(const DoremiEngine::Core::SharedContext& sharedContext);

            /**
                TOODCM test remove
            */
            void GenerateWorld(const DoremiEngine::Core::SharedContext& sharedContext);
        };
    }
}