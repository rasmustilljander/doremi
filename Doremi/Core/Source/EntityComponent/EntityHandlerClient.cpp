#include <Doremi/Core/Include/EntityComponent/EntityHandlerClient.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/LevelLoaderClient.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>
#include <Doremi/Core/Include/InterpolationHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandlerClient.hpp>

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

            // Subscribe for loading new world
            EventHandler::GetInstance()->Subscribe(EventType::LoadNewWorld, this);
        }

        EntityHandlerClient::~EntityHandlerClient() {}

        void EntityHandlerClient::RemoveEntity(int p_entityID)
        {
            EntityFactory::GetInstance()->ScrapEntity(p_entityID);
            EntityManager::GetInstance()->RemoveEntity(p_entityID);
        }


        void EntityHandlerClient::ResetWorld()
        {
            EntityFactory* t_entityFactory = EntityFactory::GetInstance();
            EntityManager* t_entityManager = EntityManager::GetInstance();

            size_t NumEntities = t_entityManager->GetLastEntity();
            for(size_t i = 0; i < NumEntities; i++)
            {
                t_entityFactory->ScrapEntity(i);
                t_entityManager->RemoveEntity(i);
            }

            t_entityManager->Reset();

            static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->RemovePlayer();
            InterpolationHandler* t_interpolationHandler = InterpolationHandler::GetInstance();
            t_interpolationHandler->Reset();

            uint32_t t_numEvents = static_cast<EventHandlerClient*>(EventHandler::GetInstance())->GetNumberOfEvents();
            cout << "Num events on reset:" << t_numEvents << endl;
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
                    cout << "Created Network player" << endl;
                }
                else if(p_entityCreated->bluepirnt == Blueprints::PlayerEntity)
                {
                    // Create entity
                    EntityID t_entityID = EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position,
                                                                      p_entityCreated->orientation, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

                    // Create player
                    static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->SetNewPlayerEntityID(t_entityID);

                    cout << "Created my player" << endl;
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
            else if(p_event->eventType == EventType::LoadNewWorld)
            {
                // Reset world
                ResetWorld();

                // Load new world
                // TODO change load based on map in event
                LevelLoaderClient t_levelLoader = LevelLoaderClient(m_sharedContext);
                t_levelLoader.LoadLevel("Levels/IntroScene.drm");

                TimeHandler* t_timeHandler = TimeHandler::GetInstance();
                t_timeHandler->ResetTime();
            }
        }
    }
}