// Project specific
#include <Server.hpp>

// Engine
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>

// Game
#include <Doremi/Core/Include/GameCore.hpp>
#include <Doremi/Core/Include/EventHandler/Events/SpecialEntityCreatedEvent.hpp>
#include <Doremi/Core/Include/TemplateCreator.hpp>
#include <Doremi/Core/Include/LevelLoaderServer.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <Doremi/Core/Include/HealthChecker.hpp>

// Components
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PlatformPatrolComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PotentialFieldComponent.hpp>

// Handlers
#include <Doremi/Core/Include/EntityComponent/EntityHandlerServer.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandlerServer.hpp>
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>
#include <Doremi/Core/Include/TreeCreator.hpp>

// Managers
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/Network/NetworkManagerServer.hpp>
#include <Doremi/Core/Include/Manager/MovementManagerServer.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <Doremi/Core/Include/Manager/FrequencyAffectedObjectManager.hpp>
#include <Doremi/Core/Include/Manager/EntitySpawnManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AIPathManager.hpp>
#include <Doremi/Core/Include/Manager/CharacterControlSyncManager.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
#include <Doremi/Core/Include/Manager/TriggerManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AITargetManager.hpp>
#include <Doremi/Core/Include/Manager/DamageManager.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
#include <Doremi/Core/Include/Manager/JumpManager.hpp>
#include <Doremi/Core/Include/Manager/GravityManager.hpp>
#include <Doremi/Core/Include/Manager/ExtraDrainSyncManager.hpp>
#include <Doremi/Core/Include/Manager/PressureParticleManager.hpp>
#include <Doremi/Core/Include/Manager/GroundEffectManagerServer.hpp>
#include <Doremi/Core/Include/Manager/LifeTimeManager.hpp>


// other
#include <Doremi/Core/Include/LevelLoaderServer.hpp>

// Timer
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

// Third party
#include <DirectXMath.h>

// Standard libraries
#include <stdexcept>
#include <exception>
#include <chrono>
#include <iostream>


namespace Doremi
{
    using namespace Core;

    ServerMain::ServerMain() {}

    ServerMain::~ServerMain() {}

    void ServerMain::Initialize()
    {
        TIME_FUNCTION_START
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(
            DoremiEngine::Core::EngineModuleEnum::NETWORK | DoremiEngine::Core::EngineModuleEnum::PHYSICS | DoremiEngine::Core::EngineModuleEnum::AI);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/


        Core::EntityFactory::StartupEntityFactory(sharedContext);
        Core::EventHandlerServer::StartupEventHandlerServer();
        Core::EntityHandlerServer::StartupEntityHandlerServer();
        Core::PlayerHandlerServer::StartPlayerHandlerServer(sharedContext);
        Core::PlayerSpawnerHandler::StartupPlayerSpawnerHandler(sharedContext);

        ////////////////Example only////////////////
        // Create manager

        // Manager* t_physicsManager = new ExampleManager(sharedContext);
        Core::LevelLoaderServer* t_levelLoader = new Core::LevelLoaderServer(sharedContext);
        t_levelLoader->LoadLevel("Levels/TestSceneCulling.drm");

        Core::Manager* t_serverNetworkManager = new Core::NetworkManagerServer(sharedContext);
        Core::Manager* t_movementManager = new Core::MovementManagerServer(sharedContext);
        Core::Manager* t_rigidTransSyndManager = new Core::RigidTransformSyncManager(sharedContext);
        Core::Manager* t_entitySpawnManager = new Core::EntitySpawnManager(sharedContext);

        Core::Manager* t_aiPathManager = new Core::AIPathManager(sharedContext);
        Core::Manager* t_aiTargetManager = new Core::AITargetManager(sharedContext);
        Core::Manager* t_charSyncManager = new Core::CharacterControlSyncManager(sharedContext); // TODO check if needed
        Core::Manager* t_jumpManager = new Core::JumpManager(sharedContext);
        Core::Manager* t_gravManager = new Core::GravityManager(sharedContext);
        Core::Manager* t_frequencyAffectedObjectManager = new Core::FrequencyAffectedObjectManager(sharedContext);
        Core::Manager* t_triggerManager = new Core::TriggerManager(sharedContext);
        Core::Manager* t_damageManager = new Core::DamageManager(sharedContext);
        Core::Manager* t_extraDrainManager = new Core::ExtraDrainSyncManager(sharedContext);
        Core::Manager* t_pressureParticleManager = new Core::PressureParticleManager(sharedContext);
        Core::Manager* t_groundEffectManagerServer = new Core::GroundEffectManagerServer(sharedContext);
        Core::Manager* t_lifeTimeManager = new Core::LifeTimeManager(sharedContext);

        // Add manager to list of managers
        // Remember to put server last (cause we want on same frame as we update to send data, or at least close togeather)
        // m_managers.push_back(t_physicsManager);
        m_managers.push_back(t_serverNetworkManager);
        // m_managers.push_back(t_extraDrainManager);
        m_managers.push_back(t_rigidTransSyndManager);
        m_managers.push_back(t_pressureParticleManager);
        m_managers.push_back(t_groundEffectManagerServer);
        m_managers.push_back(t_aiPathManager);
        m_managers.push_back(t_aiTargetManager); // Must be before movement
        m_managers.push_back(t_jumpManager);
        m_managers.push_back(t_gravManager);
        m_managers.push_back(t_frequencyAffectedObjectManager);
        m_managers.push_back(t_triggerManager);
        m_managers.push_back(t_movementManager); // Must be after gravity/jump
        m_managers.push_back(t_charSyncManager); // Must be after movement
        m_managers.push_back(t_damageManager);
        m_managers.push_back(t_entitySpawnManager);
        m_managers.push_back(t_lifeTimeManager); // Shoul probably be last or atleast on of the last

        // GenerateWorld(sharedContext);
        // GenerateWorldServerJawsTest(sharedContext);

        Core::TemplateCreator::GetInstance()->CreateTemplatesForServer(sharedContext);
        SpawnDebugWorld(sharedContext);

        ////////////////End Example////////////////

        // Remove later, needed to see something when we play solo cause of camera interactions with input
        // Doremi::Core::InputHandlerClient* inputHandler = new Doremi::Core::InputHandlerClient(sharedContext);
        // Core::PlayerHandler::GetInstance()->CreateNewPlayer(300, (Doremi::Core::InputHandler*)inputHandler);
        TIME_FUNCTION_STOP
    }

    void ServerMain::SpawnDebugWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        TIME_FUNCTION_START
        Core::EntityFactory& t_entityFactory = *Core::EntityFactory::GetInstance();

        for(size_t i = 0; i < 1; i++)
        {
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f);
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            int entityID = t_entityFactory.CreateEntity(Blueprints::PlatformEntity, position, orientation);


            // sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyAngularVelocity(entityID, XMFLOAT3(0.0f, 0.0f, 0.0f));
            // sharedContext.GetPhysicsModule().GetRigidBodyManager().SetKinematicActor(entityID, true);

            Core::PlatformPatrolComponent* t_platformPatrolComponent =
                Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PlatformPatrolComponent>(entityID);
            t_platformPatrolComponent->startPosition = position;
            t_platformPatrolComponent->endPosition = DirectX::XMFLOAT3(position.x, position.y + 140.0f, position.z);
        }

        // Create some enemies
        for(size_t i = 0; i < 0; i++)
        {
            XMFLOAT3 position = DirectX::XMFLOAT3(-2 * (int)i + 0, 0 * (int)i + 4, -2 * (int)i + 15); //-2,6,60 -280, 150.0f, -85
            XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

            int entityID = t_entityFactory.CreateEntity(Blueprints::RangedEnemyEntity, position, orientation);

            Core::SpecialEntityCreatedEvent* AIGroupActorCreated = new Core::SpecialEntityCreatedEvent(entityID, Core::EventType::RangedEnemyCreated);
            Core::EventHandler::GetInstance()->BroadcastEvent(AIGroupActorCreated);
        }

        // Create an enemy spawner
        // XMFLOAT3 position = XMFLOAT3(0, 15, 0);
        // int entityID = t_entityFactory.CreateEntity(Blueprints::EnemySpawnerEntity);
        // if(Core::EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::Transform))
        //{
        //    Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::TransformComponent>(entityID)->position = position;
        //}
        TIME_FUNCTION_STOP
    }

    void ServerMain::Run()
    {
        TIME_FUNCTION_START

        TimeHandler* t_timeHandler = TimeHandler::GetInstance();

        t_timeHandler->PreviousClock = std::chrono::high_resolution_clock::now();

        while(true)
        {
            // Tick time
            t_timeHandler->Tick();

            // Loop as many update-steps we will take this frame
            while(t_timeHandler->ShouldUpdateFrame())
            {
                // Update Game logic
                UpdateGame(t_timeHandler->UpdateStepLen);

                // Update accumulator and gametime
                t_timeHandler->UpdateAccumulatorAndGameTime();
            }
        }
        TIME_FUNCTION_STOP
    }

    void ServerMain::UpdateGame(double p_deltaTime)
    {
        TIME_FUNCTION_START
        // Deliver basic events
        static_cast<Core::EventHandlerServer*>(Core::EventHandler::GetInstance())->DeliverBasicEvents();

        // Check add removes of events health low etc..
        Core::HealthChecker::GetInstance()->Update();

        // Deliver remove events
        static_cast<Core::EventHandlerServer*>(Core::EventHandler::GetInstance())->DeliverRemoveEvents();


        Core::PlayerHandler::GetInstance()->Update(p_deltaTime);

        // TODORT
        // TODOLOG
        // Utility::Timer::MeasureTimer& timer = Utility::Timer::MeasureTimer::GetInstance();
        // Have all managers update
        size_t length = m_managers.size();
        for(size_t i = 0; i < length; i++)
        {
            // TODORT
            // TODOLOG
            // Utility::Timer::MeasureInfo& info = timer.GetTimer(m_managers.at(i)->GetName());
            // info.Reset().Start();
            m_managers.at(i)->Update(p_deltaTime);
            // info.Stop();
        }

        TIME_FUNCTION_STOP
    }

    void ServerMain::Start()
    {
        TIME_FUNCTION_START
        Initialize();
        Run();
        TIME_FUNCTION_STOP
    }
}