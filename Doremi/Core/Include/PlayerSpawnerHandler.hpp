#pragma once
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

#include <vector>

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

        /**
            Stores latest spawn point
        */
        class PlayerSpawnerHandler : public Subscriber
        {
        public:
            static PlayerSpawnerHandler* GetInstance();

            static void StartupPlayerSpawnerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                Returns the id of the latest spawn point
            */
            EntityID GetCurrentSpawnerEntityID();

            /**
                Set the ID of a spawner as current spawnpoint
            */
            void SetCurrentSpawner(EntityID p_entityID);

            /**
                Respawns player on last spawnpoint, throws event
            */
            void RespawnPlayer(EntityID p_entityID);

            /**
                Events checking for triggering spawn point
            */
            void OnEvent(Event* p_event) override;

            /**
            Adds an entity to the list of spawners*/
            void AddSpawner(EntityID p_entityID, int p_spawnerID);

            /**
            Debug method to set the current spawner based on its position
            in the member variable list*/
            void SetCurrentSpawnerDebug(int p_spawnerIndex);


        private:
            explicit PlayerSpawnerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~PlayerSpawnerHandler();

            static PlayerSpawnerHandler* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            EntityID m_currentPlayerSpawner;

            std::vector<EntityID> m_spawners;
        };
    }
}