#include <Doremi/Core/Include/EntityComponent/EntityHandlerClient.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <EntityComponent/EntityManager.hpp>

namespace Doremi
{
    namespace Core
    {
        void EntityHandlerClient::StartupEntityHandlerClient()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new EntityHandlerClient();
            }
        }

        EntityHandlerClient::EntityHandlerClient()
        {
            // Subscribing on add and remove entity
            EventHandler::GetInstance()->Subscribe(EventType::EntityCreated, this);
        }

        EntityHandlerClient::~EntityHandlerClient() {}

        void EntityHandlerClient::RemoveEntity(int p_entityID) { EntityManager::GetInstance()->RemoveEntity(p_entityID); }

        void EntityHandlerClient::OnEvent(Event* p_event)
        {
            // Call base
            EntityHandler::OnEvent(p_event);

            if(p_event->eventType == EventType::EntityCreated)
            {
                EntityCreatedEvent* p_entityCreated = (EntityCreatedEvent*)p_event;

                // Create entity
                EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position);
            }
        }
    }
}