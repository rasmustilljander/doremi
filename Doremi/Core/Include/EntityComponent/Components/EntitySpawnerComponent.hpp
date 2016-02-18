#pragma once
#include <EntityComponent/Constants.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Enum used to identify which type of type the entityspawner
        is. Important for the manager to determine which logic it
        uses. Could probably do some fancy inheritence soludion but
        this works just fine... I think*/
        enum class SpawnerType
        {
            EventSpawner = 0x10, // Spawns entities on event
            TimedSpawner = 0x20, // Spawns entities after a time
        };

        /**
        Used to spawn any type of entity. The component has no idea
        which objects triggers it: this filtereing is handled by the
        manager. Might be a good idea to change this behaviour.
        */
        struct EntitySpawnComponent
        {
            // The type of this specific spawner
            SpawnerType type;
            // The entity we want to spawn
            Blueprints entityBlueprint;
            // Number of entities to be spawned from this spawner
            int maxNumSpawnedEntites;
            // Current number of entities that have been spawned
            int currentNumSpawnedEntities = 0;
            // How many that can be alive at once
            int maxAlive = 0;
            // How many that are alive
            int currentNumAlive = 0;
            // Used by timed spawner
            float timeBetweenSpawns;
            float timeSinceLastSpawn = 0;
            // Used to filter out if thingies are too far away
            float spawnRadius;
        };
    }
}