#pragma once

// Project specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <Doremi/Core/Include/EntityComponent/ComponentTable.hpp>
#include <Doremi/Core/Include/EntityComponent/StorageShelf.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        class EntityHandler : public Subscriber
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
                Also returns EntityID of newly created entity
            */
            virtual int CreateEntity(Blueprints p_blueprintID);

            /**
                Creates an entity according to the provided blueprint id
                Also returns EntityID of newly created entity
            */
            virtual int CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position);

            /**
                Creates an entity according to the provided blueprint id
                Also returns EntityID of newly created entity
            */
            virtual int CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation);

            /** Returns desired component. Example: GetComponentFromStorage<ComponentName>(id);*/
            template <class T> T* GetComponentFromStorage(EntityID p_id) { return GetComponent<T>(p_id); }

            /** Adds a new component to the bit mask*/
            void AddComponent(int p_entityID, int p_mask);

            /** Removes the entitys component*/
            void RemoveComponent(int p_entityID, int p_mask);

            /**
                Removes the entire entity at the specific location
            */
            virtual void RemoveEntity(int p_entityID) = 0;

            /**
                Checks events for add and remove objects
            */
            virtual void OnEvent(Event* p_event) override;

        protected:
            EntityHandler();
            ~EntityHandler();
            static EntityHandler* m_singleton;
        };
    }
}