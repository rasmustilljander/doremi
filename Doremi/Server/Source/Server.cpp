// Project specific
#include <Server.hpp>
#include <Doremi/Core/Include/GameCore.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
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
        const DoremiEngine::Core::SharedContext& sharedContext =
            InitializeEngine(DoremiEngine::Core::EngineModuleEnum::NETWORK | DoremiEngine::Core::EngineModuleEnum::PHYSICS);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/

        Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();
        Core::PlayerHandler::StartPlayerHandler(sharedContext);

        ////////////////Example only////////////////
        // Create manager

        // Manager* t_physicsManager = new ExampleManager(sharedContext);
        Core::Manager* t_serverNetworkManager = new Core::ServerNetworkManager(sharedContext);
        Core::Manager* t_movementManager = new Core::MovementManager(sharedContext);
        Core::Manager* t_rigidTransSyndManager = new Core::RigidTransformSyncManager(sharedContext);

        // Add manager to list of managers
        // Remember to put server last (cause we want on same frame as we update to send data, or at least close togeather)
        // m_managers.push_back(t_physicsManager);
        m_managers.push_back(t_serverNetworkManager);
        m_managers.push_back(t_rigidTransSyndManager);

        m_managers.push_back(t_movementManager);

        // GenerateWorld(sharedContext);
        // GenerateWorldServerJawsTest(sharedContext);

        Core::TemplateCreator::GetInstance()->CreateTemplatesForServer(sharedContext);
        SpawnDebugWorld(sharedContext);

        ////////////////End Example////////////////
    }

    void ServerMain::SpawnDebugWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();

        // Create entity
        int playerID = t_entityHandler.CreateEntity(Blueprints::PlayerEntity);

        // Create the rigid body
        int materialID = t_entityHandler.GetComponentFromStorage<Core::PhysicsMaterialComponent>(playerID)->p_materialID;
        DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 10, 0);
        DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
        Core::RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<Core::RigidBodyComponent>(playerID);
        bodyComp->p_bodyID =
            sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(position, orientation, DirectX::XMFLOAT3(0.5, 0.5, 0.5), materialID);

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
        for(size_t i = 0; i < 5; i++)
        {
            int entityID = t_entityHandler.CreateEntity(Blueprints::PlatformEntity);
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 10 - (int)i, i * 5);
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            Core::RigidBodyComponent* rigidComp = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::RigidBodyComponent>(entityID);
            rigidComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(position, orientation, XMFLOAT3(2, 0.05, 2), matID);
        }
    }

    void JawsSimulatePhysicsDebug(double deltaTime)
    {
        Core::TransformComponent* trans = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::TransformComponent>(1);

        if(trans->position.x == 1)
        {
            trans->position.y += deltaTime;
            if(trans->position.y > 5)
            {
                trans->position.x = -1;
            }
        }
        else
        {
            trans->position.y -= deltaTime;
            if(trans->position.y < -5)
            {
                trans->position.x = 1;
            }
        }
        DirectX::XMStoreFloat4(&trans->rotation, DirectX::XMQuaternionRotationRollPitchYaw(0, 0, trans->position.y * 1.0f));
    }

    void ServerMain::Run()
    {
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
    }

    void ServerMain::UpdateGame(double p_deltaTime)
    {
        Core::EventHandler::GetInstance()->DeliverEvents();
        Core::PlayerHandler::GetInstance()->UpdatePlayerPositions();
        Core::PlayerHandler::GetInstance()->UpdatePlayerRotationsServer();

        // Have all managers update
        size_t length = m_managers.size();
        for(size_t i = 0; i < length; i++)
        {
            m_managers.at(i)->Update(p_deltaTime);
        }
    }

    void ServerMain::Start()
    {
        // Start the game
        /*Core::GameCore* gameCore = new Core::GameCore();
        gameCore->InitializeServer();
        gameCore->StartServerCore();*/

        Initialize();

        Run();
    }
}