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
#include <Doremi/Core/Include/ScreenSpaceDrawer.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <Doremi/Core/Include/MenuClasses/LoadingScreenHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/AudioComponent.hpp>

#include <DoremiEngine/Audio/Include/AudioModule.hpp>

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
            // Play death sound i there is any, This should be done at a better place TODOKO
            if(HasComponents(p_entityID, (int)ComponentType::Audio))
            {
                int channel = -1; // Will be thrown away later...
                AudioComponent* audioComp = GetComponentFromStorage<AudioComponent>(p_entityID);
                m_sharedContext.GetAudioModule().PlayASound(audioComp->m_enumToSoundID[(int32_t)AudioCompEnum::Death],
                                                            audioComp->m_enumToLoop[(int32_t)AudioCompEnum::Death], channel, DoremiEngine::Audio::SoundGroup::Effect);
                m_sharedContext.GetAudioModule().SetVolumeOnChannel(channel, 1.0f);
            }

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
        }

        void EntityHandlerClient::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::EntityCreated)
            {
                EntityCreatedEvent* p_entityCreated = static_cast<EntityCreatedEvent*>(p_event);

                if(p_entityCreated->bluepirnt == Blueprints::NetworkPlayerEntity)
                {
                    // Create entity
                    EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position, DirectX::XMFLOAT4(0, 0, 0, 1));
                    cout << "Created Network player" << endl;
                }
                else if(p_entityCreated->bluepirnt == Blueprints::PlayerEntity)
                {
                    // Create entity
                    EntityID t_entityID = EntityHandler::CreateEntity(p_entityCreated->bluepirnt, p_entityCreated->position, p_entityCreated->orientation);

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
                RemoveEntityEvent* p_removeEvent = static_cast<RemoveEntityEvent*>(p_event);

                RemoveEntity(p_removeEvent->entityID);
            }
            else if(p_event->eventType == EventType::LoadNewWorld)
            {
                LoadingScreenHandler::GetInstance()->Reset();

                // Start draw a load screen
                // Mohaha hax


                // Restart case we want a loading screen
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().BeginDraw();
                ScreenSpaceDrawer::GetInstance()->DrawLoadingScreen();
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().BeginDraw();

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