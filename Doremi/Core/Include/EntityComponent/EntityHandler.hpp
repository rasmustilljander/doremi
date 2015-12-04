#pragma once

// Project specific
#include <EntityComponent/Constants.hpp>
#include <EntityComponent/ComponentTable.hpp>
#include <EntityComponent/StorageShelf.hpp>

namespace Doremi
{
    namespace Core
    {
        class EntityHandler
        {
            public:
            static EntityHandler& GetInstance();

            /** Registers a blueprint for entity creation with an ID*/
            void RegisterEntityBlueprint(Blueprints p_blueprintID, EntityBlueprint p_blueprint);

            /** Get index of last entity (useful for for-loops over all entities)*/
            int GetLastEntityIndex();

            /** Checks if the entity specified has the entities in the mask. Mask has to be created manually*/
            bool HasComponents(EntityID p_id, int p_mask);

            /** 
            Creates an entity according to the provided blueprint id
            Also returns EntityID of newly created entity*/
            int CreateEntity(Blueprints p_blueprintID);

            /** Returns desired component. Example: GetComponentFromStorage<ComponentName>(id);*/
            template <class T> T* GetComponentFromStorage(EntityID p_id)
            {
                return GetComponent<T>(p_id);
            }

            /** Adds a new component to the bit mask*/
            void AddComponent(int p_entityID, int p_mask);

            /** Removes the entitys component*/
            void RemoveComponent(int p_entityID, int p_mask);

            /** Removes the entire entity at the specific location*/
            void RemoveEntity(int p_entityID);
        private:
            EntityHandler();
            ~EntityHandler();
            static EntityHandler* m_singleton;
        };
    }
}