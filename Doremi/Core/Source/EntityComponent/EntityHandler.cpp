// Project specific
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/StorageShelf.hpp>
#include <EntityComponent/ComponentTable.hpp>

namespace Doremi
{
    namespace Core
    {
        EntityHandler* EntityHandler::m_singleton = nullptr;

        EntityHandler::EntityHandler() {}

        EntityHandler::~EntityHandler() {}

        EntityHandler& EntityHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new EntityHandler();
            }
            return *m_singleton;
        }

        void EntityHandler::RegisterEntityBlueprint(Blueprints p_blueprintID, EntityBlueprint p_blueprint)
        {
            EntityFactory::GetInstance()->RegisterEntityTemplate(p_blueprintID, p_blueprint);
        }

        int EntityHandler::GetLastEntityIndex()
        {
            return EntityManager::GetInstance()->GetLastEntity();
        }

        bool EntityHandler::HasComponents(EntityID p_id, int p_mask)
        {
            return ComponentTable::GetInstance()->HasComponent(p_id, p_mask);
        }

        int EntityHandler::CreateEntity(Blueprints p_blueprintID)
        {
            return EntityFactory::GetInstance()->CreateEntity(p_blueprintID);
        }

        void EntityHandler::AddComponent(int p_entityID, int p_mask)
        {
            ComponentTable::GetInstance()->AddComponent(p_entityID, p_mask);
        }

        void EntityHandler::RemoveComponent(int p_entityID, int p_mask)
        {
            ComponentTable::GetInstance()->RemoveComponent(p_entityID, p_mask);
        }

        void EntityHandler::RemoveEntity(int p_entityID)
        {
            EntityManager::GetInstance()->RemoveEntity(p_entityID);
        }
    }
}