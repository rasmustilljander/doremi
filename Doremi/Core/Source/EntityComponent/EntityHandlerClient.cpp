#include <Doremi/Core/Include/EntityComponent/EntityHandlerClient.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#include <iostream>

namespace Doremi
{
    namespace Core
    {
        void EntityHandlerClient::StartupEntityHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new EntityHandlerClient(p_sharedContext);
            }
        }

        EntityHandlerClient::EntityHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            // Subscribing on add entity
            EventHandler::GetInstance()->Subscribe(EventType::EntityCreated, this);

            // Subscribing on remove entity
            EventHandler::GetInstance()->Subscribe(EventType::RemoveEntity, this);
        }

        EntityHandlerClient::~EntityHandlerClient() {}

        void EntityHandlerClient::RemoveEntity(int p_entityID)
        {
            EntityFactory::GetInstance()->ScrapEntity(p_entityID);
            EntityManager::GetInstance()->RemoveEntity(p_entityID);
        }

        void EntityHandlerClient::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::EntityCreated)
            {
                EntityCreatedEvent* p_entityCreated = (EntityCreatedEvent*)p_event;

                if(p_entityCreated->bluepirnt == Blueprints::NetworkPlayerEntity)
                {
                    // Create entity
                    EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position, DirectX::XMFLOAT4(0, 0, 0, 1),
                                                DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
                }
                else if(p_entityCreated->bluepirnt == Blueprints::PlayerEntity)
                {
                    // Create entity
                    EntityID t_entityID = EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position,
                                                                      p_entityCreated->orientation, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

                    // Create player
                    static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->SetNewPlayerEntityID(t_entityID);
                }
                else
                {
                    // Create entity
                    EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position);
                }
            }
            else if(p_event->eventType == EventType::RemoveEntity)
            {
                RemoveEntityEvent* p_removeEvent = (RemoveEntityEvent*)p_event;

                RemoveEntity(p_removeEvent->entityID);
            }
        }
    }
}