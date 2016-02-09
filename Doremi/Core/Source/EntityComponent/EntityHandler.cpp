// Project specific
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/StorageShelf.hpp>
#include <EntityComponent/ComponentTable.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>

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
                std::runtime_error("Get instance used on unitialized EntityHandler");
            }
            return *m_singleton;
        }

        void EntityHandler::RegisterEntityBlueprint(Blueprints p_blueprintID, EntityBlueprint p_blueprint)
        {
            EntityFactory::GetInstance()->RegisterEntityTemplate(p_blueprintID, p_blueprint);
        }

        int EntityHandler::GetLastEntityIndex() { return EntityManager::GetInstance()->GetLastEntity(); }

        bool EntityHandler::HasComponents(EntityID p_id, int p_mask) { return ComponentTable::GetInstance()->HasComponent(p_id, p_mask); }

        int EntityHandler::CreateEntity(Blueprints p_blueprintID) { return EntityFactory::GetInstance()->CreateEntity(p_blueprintID); }

        int EntityHandler::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position)
        {
            return EntityFactory::GetInstance()->CreateEntity(p_blueprintID, p_position);
        }

        int EntityHandler::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation)
        {
            return EntityFactory::GetInstance()->CreateEntity(p_blueprintID, p_position, p_orientation);
        }

        int EntityHandler::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_scale)
        {
            return EntityFactory::GetInstance()->CreateEntity(p_blueprintID, p_position, p_orientation, p_scale);
        }


        void EntityHandler::AddComponent(int p_entityID, int p_mask) { ComponentTable::GetInstance()->AddComponent(p_entityID, p_mask); }

        void EntityHandler::RemoveComponent(int p_entityID, int p_mask) { ComponentTable::GetInstance()->RemoveComponent(p_entityID, p_mask); }
    }
}