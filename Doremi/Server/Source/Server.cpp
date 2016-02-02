// Project specific
#include <Server.hpp>
#include <Doremi/Core/Include/GameCore.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandlerServer.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/Manager/Network/ServerNetworkManager.hpp>
#include <Doremi/Core/Include/Manager/MovementManagerServer.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <Doremi/Core/Include/Manager/FrequencyAffectedObjectManager.hpp>
#include <Doremi/Core/Include/Manager/EntitySpawnManager.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <Doremi/Core/Include/TemplateCreator.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PlatformPatrolComponent.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AIPathManager.hpp>
#include <Doremi/Core/Include/Manager/CharacterControlSyncManager.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PotentialFieldComponent.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <Doremi/Core/Include/Manager/JumpManager.hpp>
#include <Doremi/Core/Include/Manager/GravityManager.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <Doremi/Core/Include/LevelLoaderServer.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
#include <Doremi/Core/Include/Manager/TriggerManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AITargetManager.hpp>
#include <Doremi/Core/Include/Manager/DamageManager.hpp>
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
    ServerMain::ServerMain() {}

    ServerMain::~ServerMain() {}

    void ServerMain::Initialize()
    {
        TIME_FUNCTION_START
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(
            DoremiEngine::Core::EngineModuleEnum::NETWORK | DoremiEngine::Core::EngineModuleEnum::PHYSICS | DoremiEngine::Core::EngineModuleEnum::AI);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/

        Core::EntityHandlerServer::StartupEntityHandlerServer();
        Core::PlayerHandler::StartPlayerHandler(sharedContext);

        ////////////////Example only////////////////
        // Create manager

        // Manager* t_physicsManager = new ExampleManager(sharedContext);
        Core::LevelLoaderServer* t_levelLoader = new Core::LevelLoaderServer(sharedContext);
        t_levelLoader->LoadLevel("Levels/test.drm");
        Core::Manager* t_serverNetworkManager = new Core::ServerNetworkManager(sharedContext);
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

        // Add manager to list of managers
        // Remember to put server last (cause we want on same frame as we update to send data, or at least close togeather)
        // m_managers.push_back(t_physicsManager);
        m_managers.push_back(t_serverNetworkManager);
        m_managers.push_back(t_rigidTransSyndManager);
        m_managers.push_back(t_aiPathManager);
        m_managers.push_back(t_aiTargetManager);
        m_managers.push_back(t_jumpManager);
        m_managers.push_back(t_gravManager);
        m_managers.push_back(t_frequencyAffectedObjectManager);
        m_managers.push_back(t_triggerManager);
        m_managers.push_back(t_movementManager); // Must be after gravity/jump
        m_managers.push_back(t_charSyncManager); // Must be after movement
        m_managers.push_back(t_damageManager);
        m_managers.push_back(t_entitySpawnManager);

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
            int entityID = t_entityFactory.CreateEntity(Blueprints::PlatformEntity);
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f);
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            Core::RigidBodyComponent* rigidComp = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::RigidBodyComponent>(entityID);
            float factor = 2.5f;
            rigidComp->p_bodyID =
                sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation,
                                                                                         XMFLOAT3(2 * factor, 0.05 * factor, 2 * factor), matID);
            sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyAngularVelocity(entityID, XMFLOAT3(0.0f, 0.0f, 0.0f));
            sharedContext.GetPhysicsModule().GetRigidBodyManager().SetKinematicActor(entityID, true);

            Core::PlatformPatrolComponent* t_platformPatrolComponent =
                Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PlatformPatrolComponent>(entityID);
            t_platformPatrolComponent->startPosition = position;
            t_platformPatrolComponent->endPosition = DirectX::XMFLOAT3(position.x, position.y + 140.0f, position.z);
        }

        // Create some enemies
        for(size_t i = 0; i < 1; i++)
        {
            XMFLOAT3 position = DirectX::XMFLOAT3(-280 * (int)i + 0, 140 * (int)i + 4, -85 * (int)i + 15); //-2,6,60 -280, 150.0f, -85
            XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            int entityID = t_entityFactory.CreateEntity(Blueprints::EnemyEntity, position, orientation);
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(entityID, matID, position, XMFLOAT2(0.1f, 0.5f));

            Core::PotentialFieldComponent* potentialComponent =
                Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PotentialFieldComponent>(entityID);
            potentialComponent->ChargedActor = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(position, -1.0f, 3.0f, false);

            Core::EntityCreatedEvent* AIGroupActorCreated = new Core::EntityCreatedEvent(entityID, Core::EventType::AiGroupActorCreation);
            Core::EventHandler::GetInstance()->BroadcastEvent(AIGroupActorCreated);
        }
        TIME_FUNCTION_STOP
    }

    void JawsSimulatePhysicsDebug(double deltaTime)
    {
        TIME_FUNCTION_START

        TIME_FUNCTION_STOP
    }

    void ServerMain::Run()
    {
        TIME_FUNCTION_START
        std::chrono::time_point<std::chrono::high_resolution_clock> CurrentClock, PreviousClock;
        PreviousClock = std::chrono::high_resolution_clock::now();

        double Frame = 0;
        double Offset = 0;
        double Accum = 0;
        double GameTime = 0;
        double UpdateStepLen = 0.017;
        double MaxFrameTime = 0.25;

        while(true)
        {
            CurrentClock = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = (CurrentClock - PreviousClock);
            Frame = duration.count() + Offset;
            Offset = 0;


            // We simulate maximum 250 milliseconds each frame
            // If we would let it be alone we would get mayor stops instead of lesser ones that will slowly catch up
            if(Frame > MaxFrameTime)
            {
                Offset = Frame - MaxFrameTime;
                Frame = MaxFrameTime;
                cout << "Frame took more then " << MaxFrameTime << " Seconds" << endl;
            }

            // Update the previous position with frametime so we can catch up if we slow down
            PreviousClock = CurrentClock;

            // Update Accumulator (how much we will work this frame)
            Accum += Frame;

            // Loop as many update-steps we will take this frame
            while(Accum >= UpdateStepLen)
            {
                // Update Game logic
                JawsSimulatePhysicsDebug(UpdateStepLen); // TODOCM remove
                // Update Game logic
                UpdateGame(UpdateStepLen);

                // Remove time from accumulator
                // Accumulator -= UpdateTimeStepLength;
                Accum -= UpdateStepLen;

                // Add time to start
                GameTime += UpdateStepLen;
            }
        }
        TIME_FUNCTION_STOP
    }

    void ServerMain::UpdateGame(double p_deltaTime)
    {
        TIME_FUNCTION_START
        Core::EventHandler::GetInstance()->DeliverEvents();
        Core::PlayerHandler::GetInstance()->UpdateServer(p_deltaTime);

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