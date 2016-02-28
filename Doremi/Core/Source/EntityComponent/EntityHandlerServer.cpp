#include <Doremi/Core/Include/EntityComponent/EntityHandlerServer.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityManager.hpp>

namespace Doremi
{
    namespace Core
    {
        EntityHandlerServer::EntityHandlerServer()
        {
            // Subscribing on remove entity
            EventHandler::GetInstance()->Subscribe(EventType::RemoveEntity, this);
        }

        EntityHandlerServer::~EntityHandlerServer() {}

        void EntityHandlerServer::StartupEntityHandlerServer()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new EntityHandlerServer();
            }
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID);

            EventHandler::GetInstance()->BroadcastEvent(new EntityCreatedEvent(outID, p_blueprintID));

            return outID;
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID, p_position);

            EventHandler::GetInstance()->BroadcastEvent(new EntityCreatedEvent(outID, p_blueprintID, p_position, DirectX::XMFLOAT4(0, 0, 0, 1)));

            return outID;
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID, p_position, p_orientation);

            EventHandler::GetInstance()->BroadcastEvent(new EntityCreatedEvent(outID, p_blueprintID, p_position, p_orientation));

            return outID;
        }

        int EntityHandlerServer::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_scale)
        {
            EntityID outID = EntityHandler::CreateEntity(p_blueprintID, p_position, p_orientation, p_scale);

            EventHandler::GetInstance()->BroadcastEvent(new EntityCreatedEvent(outID, p_blueprintID, p_position, p_orientation));

            return outID;
        }

        void EntityHandlerServer::RemoveEntity(int p_entityID) { EventHandler::GetInstance()->BroadcastEvent(new RemoveEntityEvent(p_entityID)); }


        void EntityHandlerServer::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::RemoveEntity)
            {
                RemoveEntityEvent* p_removeEvent = (RemoveEntityEvent*)p_event;
                EntityFactory::GetInstance()->ScrapEntity(p_removeEvent->entityID);
                EntityManager::GetInstance()->RemoveEntity(p_removeEvent->entityID);
            }
        }
    }
}
