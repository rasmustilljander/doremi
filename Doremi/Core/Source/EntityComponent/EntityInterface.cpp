// Project specific
#include <EntityComponent/EntityInterface.hpp>
#include <EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/StorageShelf.hpp>
#include <EntityComponent/ComponentTable.hpp>

namespace Doremi
{
    namespace Core
    {
        EntityInterface* EntityInterface::m_singleton = nullptr;

        EntityInterface::EntityInterface()
        {
        }

        EntityInterface::~EntityInterface()
        {
        }

        EntityInterface* EntityInterface::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                m_singleton = new EntityInterface();
            }
            return m_singleton;
        }

        void EntityInterface::RegisterEntityBlueprint(Blueprints p_blueprintID, EntityBlueprint p_blueprint)
        {
            EntityFactory::GetInstance()->RegisterEntityTemplate(p_blueprintID, p_blueprint);
        }

        int EntityInterface::GetLastEntityIndex()
        {
            return EntityManager::GetInstance()->GetLastEntity();
        }

        bool EntityInterface::HasComponents(EntityID p_id, int p_mask)
        {
            return ComponentTable::GetInstance()->HasComponent(p_id, p_mask);
        }

        void EntityInterface::CreateEntity(Blueprints p_blueprintID)
        {
            EntityFactory::GetInstance()->CreateEntity(p_blueprintID);
        }

        void EntityInterface::AddComponent(int p_entityID, int p_mask)
        {
            ComponentTable::GetInstance()->AddComponent(p_entityID, p_mask);
        }

        void EntityInterface::RemoveComponent(int p_entityID, int p_mask)
        {
            ComponentTable::GetInstance()->RemoveComponent(p_entityID, p_mask);
        }

        void EntityInterface::RemoveEntity(int p_entityID)
        {
            EntityManager::GetInstance()->RemoveEntity(p_entityID);
        }
    }
}