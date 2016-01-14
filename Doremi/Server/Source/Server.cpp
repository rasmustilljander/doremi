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
#include <Doremi/Core/Include/Manager/MovementManager.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <Doremi/Core/Include/TemplateCreator.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
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
// Timer
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

// Third party
#include <DirectXMath.h>

// Standard libraries
#include <stdexcept>
#include <exception>
#include <chrono>

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
        Core::Manager* t_serverNetworkManager = new Core::ServerNetworkManager(sharedContext);
        Core::Manager* t_movementManager = new Core::MovementManager(sharedContext);
        Core::Manager* t_rigidTransSyndManager = new Core::RigidTransformSyncManager(sharedContext);

        Core::Manager* t_aiPathManager = new Core::AIPathManager(sharedContext);
        Core::Manager* t_charSyncManager = new Core::CharacterControlSyncManager(sharedContext); // TODO check if needed
        Core::Manager* t_jumpManager = new Core::JumpManager(sharedContext);
        Core::Manager* t_gravManager = new Core::GravityManager(sharedContext);

        // Add manager to list of managers
        // Remember to put server last (cause we want on same frame as we update to send data, or at least close togeather)
        // m_managers.push_back(t_physicsManager);
        m_managers.push_back(t_serverNetworkManager);
        m_managers.push_back(t_rigidTransSyndManager);
        m_managers.push_back(t_movementManager);
        m_managers.push_back(t_aiPathManager);
        m_managers.push_back(t_charSyncManager);
        m_managers.push_back(t_jumpManager);
        m_managers.push_back(t_gravManager);


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
        Core::LevelLoaderServer* t_levelLoader = new Core::LevelLoaderServer(sharedContext);
        t_levelLoader->LoadLevel("Levels/test.drm");
        // Create entity
        // int playerID = t_entityHandler.CreateEntity(Blueprints::PlayerEntity);

        //// Create the rigid body
        // int materialID = t_entityHandler.GetComponentFromStorage<Core::PhysicsMaterialComponent>(playerID)->p_materialID;
        // DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 10, 0);
        // DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
        // Core::RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<Core::RigidBodyComponent>(playerID);
        /*bodyComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(playerID, position, orientation,
                                                                                                      DirectX::XMFLOAT3(0.5, 0.5, 0.5), materialID);*/

        // sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(playerID, materialID, position, XMFLOAT2(1, 1));
        // Core::EntityHandler::GetInstance().AddComponent(playerID, (int)ComponentType::CharacterController);

        int entityDebugJaws = t_entityFactory.CreateEntity(Blueprints::JawsDebugEntity);
        Core::TransformComponent* trans = GetComponent<Core::TransformComponent>(entityDebugJaws);
        trans->position = DirectX::XMFLOAT3(-10, 5, 0);


        for(size_t i = 0; i < 5; i++)
        {
            int entityID = t_entityFactory.CreateEntity(Blueprints::PlatformEntity);
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 10 - (int)i, i * 5);
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            Core::RigidBodyComponent* rigidComp = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::RigidBodyComponent>(entityID);
            rigidComp->p_bodyID =
                sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation, XMFLOAT3(2, 0.05, 2), matID);
        }

        // Create some enemies
        for(size_t i = 0; i < 8; i++)
        {
            int entityID = t_entityFactory.CreateEntity(Blueprints::EnemyEntity);
            XMFLOAT3 position = DirectX::XMFLOAT3(0, 7 - (int)i, i * 5);
            XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            // RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
            // rigidComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation,
            //                                                                                               XMFLOAT3(0.5, 0.5, 0.5), matID);
            sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(entityID, matID, position, XMFLOAT2(0.1, 0.5));

            Core::PotentialFieldComponent* potentialComponent =
                Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PotentialFieldComponent>(entityID);
            potentialComponent->ChargedActor = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(DirectX::XMFLOAT3(0, 0, 0), -1, 3, false);

            Core::EntityCreatedEvent* AIGroupActorCreated = new Core::EntityCreatedEvent(entityID, Core::EventType::AiGroupActorCreation);
            Core::EventHandler::GetInstance()->BroadcastEvent(AIGroupActorCreated);
        }

        // TODO Not using this event atm, because of refac, will need to find some solution
        /*PlayerCreationEvent* playerCreationEvent = new PlayerCreationEvent();
        playerCreationEvent->eventType = Events::PlayerCreation;
        playerCreationEvent->playerEntityID = playerID;*/

        // EventHandler::GetInstance()->BroadcastEvent(playerCreationEvent);


        // for(size_t i = 0; i < 1; i++)
        //{
        //    int entityID = t_entityHandler.CreateEntity(Blueprints::PlatformEntity);
        //    XMFLOAT3 position = DirectX::XMFLOAT3(0, -2 - (int)i, i * 5);
        //    XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
        //    int matID = EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(entityID)->p_materialID;
        //    RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);/*
        //    rigidComp->p_bodyID =
        //        sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(position, orientation, XMFLOAT3(2, 0.05, 2), matID);*/
        //    rigidComp->p_bodyID =
        //        sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(position, orientation, XMFLOAT3(200, 0.05, 200), matID);
        //}
        TIME_FUNCTION_STOP
    }

    void JawsSimulatePhysicsDebug(double deltaTime)
    {
        TIME_FUNCTION_START
        Core::TransformComponent* trans = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::TransformComponent>(0);

        if(trans->position.x == -10)
        {
            trans->position.y += deltaTime;
            if(trans->position.y > 5)
            {
                trans->position.x = -11;
            }
        }
        else
        {
            trans->position.y -= deltaTime;
            if(trans->position.y < -5)
            {
                trans->position.x = -10;
            }
        }
        DirectX::XMStoreFloat4(&trans->rotation, DirectX::XMQuaternionRotationRollPitchYaw(0, 0, trans->position.y * 1.0f));
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

        while(true)
        {
            CurrentClock = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = (CurrentClock - PreviousClock);
            Frame = duration.count() + Offset;
            Offset = 0;


            // We simulate maximum 250 milliseconds each frame
            // If we would let it be alone we would get mayor stops instead of lesser ones that will slowly catch up
            if(Frame > 0.25f)
            {
                Offset = Frame - 0.25f;
                Frame = 0.25f;
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
        Core::PlayerHandler::GetInstance()->UpdateServer();

        Utility::Timer::MeasureTimer& timer = Utility::Timer::MeasureTimer::GetInstance();
        // Have all managers update
        size_t length = m_managers.size();
        for(size_t i = 0; i < length; i++)
        {
            Utility::Timer::MeasureInfo& info = timer.GetTimer(m_managers.at(i)->GetName());
            info.Reset().Start();
            m_managers.at(i)->Update(p_deltaTime);
            info.Stop();
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