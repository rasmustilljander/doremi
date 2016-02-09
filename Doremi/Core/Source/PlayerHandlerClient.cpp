#include <Doremi/Core/Include/PlayerHandlerClient.hpp>

#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>

#include <Doremi/Core/Include/NetworkEventReceiver.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>

#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/GunFireToggleEvent.hpp>

#include <DoremiEngine/Input/Include/InputModule.hpp>

// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>

// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>

// Components
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/PressureParticleComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

#include <Doremi/Core/Include/InputHandlerClient.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

namespace Doremi
{
    namespace Core
    {
        PlayerHandlerClient::PlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : PlayerHandler(p_sharedContext) {}

        PlayerHandlerClient::~PlayerHandlerClient() {}


        void PlayerHandlerClient::StartPlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("PlayerHandler StartPlayerHandler called multiple times.");
            }
            m_singleton = new PlayerHandlerClient(p_sharedContext);
        }

        void PlayerHandlerClient::Update(double p_dt)
        {
            TIME_FUNCTION_START
            UpdatePlayerPositions();
            UpdatePlayerRotationsClient();
            UpdateFiring();
            TIME_FUNCTION_STOP
        }


        void PlayerHandlerClient::UpdatePlayerInputs()
        {
            TIME_FUNCTION_START
            m_sharedContext.GetInputModule().Update();

            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                ((InputHandlerClient*)iter->second->m_inputHandler)->Update();
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandlerClient::CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);


            if(iter != m_playerMap.end())
            {
                std::runtime_error("Creating player twice with same ID.");
            }

            // TODOCM hard coded entityID for new players
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(5.0f, 30.0f, 0.0f);
            EntityID t_EntityID =
                EntityHandler::GetInstance().CreateEntity(Blueprints::PlayerEntity, position, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0.25, 0.25, 0.25));

            NetworkEventReceiver* newNetworkEventReceiver = new NetworkEventReceiver();
            FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();

            Player* NewPlayer = new PlayerClient(t_EntityID, p_inputHandler, newFrequencyHandler, newNetworkEventReceiver);

            m_playerMap[p_playerID] = NewPlayer;


            // int materialID =
            // EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(NewPlayer->m_playerEntityID)->p_materialID;
            // DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(5.0f, 30.0f, 0.0f);
            // DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            // m_sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(NewPlayer->m_playerEntityID, materialID, position,
            // XMFLOAT2(1.0f, 1.0f));

            /// Add a new potential field actor to the player
            // Check if we have a actor, different from server and client...
            if(EntityHandler::GetInstance().HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PotentialField))
            {
                PotentialFieldComponent* pfComponent = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(NewPlayer->m_playerEntityID);
                pfComponent->ChargedActor =
                    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(position, 30, 60, false, DoremiEngine::AI::AIActorType::Player);
            }
            /// Create the gun
            // Check if we have the gun
            if(EntityHandler::GetInstance().HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PressureParticleSystem))
            {
                ParticlePressureComponent* particleComp =
                    EntityHandler::GetInstance().GetComponentFromStorage<ParticlePressureComponent>(NewPlayer->m_playerEntityID);
                particleComp->data.m_active = false;
                particleComp->data.m_density = 2.0f;
                particleComp->data.m_dimensions = XMFLOAT2(0.0f, 0.0f);
                particleComp->data.m_direction = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(NewPlayer->m_playerEntityID)->rotation;
                particleComp->data.m_emissionAreaDimensions = XMFLOAT2(3.14 / 4, 3.14 / 5);
                particleComp->data.m_emissionRate = 0.05;
                particleComp->data.m_launchPressure = 100;
                particleComp->data.m_numParticlesX = 5;
                particleComp->data.m_numParticlesY = 1;
                particleComp->data.m_size = 1;
                particleComp->data.m_position = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(NewPlayer->m_playerEntityID)->position;
                m_sharedContext.GetPhysicsModule().GetFluidManager().CreateParticleEmitter(NewPlayer->m_playerEntityID, particleComp->data);
            }


            // Create event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(NewPlayer->m_playerEntityID);

            // Broadcast event
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        NetworkEventReceiver* PlayerHandlerClient::GetNetworkEventReceiverForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkEventReceiver* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = (static_cast<PlayerClient*>(iter->second))->m_networkEventReceiver;
            }

            return outPointer;
        }
        void PlayerHandlerClient::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::GunFireToggle)
            {
                GunFireToggleEvent* t_gunFireToggleEvent = static_cast<GunFireToggleEvent*>(p_event);
                if(t_gunFireToggleEvent->isFiring)
                {
                    m_gunController.StartFireGun(t_gunFireToggleEvent->entityID, m_sharedContext);
                }
                else
                {
                    m_gunController.StopFireGun(t_gunFireToggleEvent->entityID, m_sharedContext);
                }
            }
        }
    }
}
