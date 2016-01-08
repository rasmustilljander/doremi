// Project specific
#include <Game.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/InterpolationHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <Doremi/Core/Include/Manager/GraphicManager.hpp>
#include <Doremi/Core/Include/Manager/Network/ClientNetworkManager.hpp>
#include <Doremi/Core/Include/Manager/MovementManager.hpp>
#include <Doremi/Core/Include/Manager/AudioManager.hpp>
#include <Doremi/Core/Include/Manager/CameraManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AIPathManager.hpp>
#include <Doremi/Core/Include/Manager/CharacterControlSyncManager.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <Doremi/Core/Include/TemplateCreator.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

// Third party

// Standard libraries
#include <stdexcept>
#include <exception>
#include <chrono>

namespace Doremi
{
    GameMain::GameMain() {}

    GameMain::~GameMain() {}

    void GameMain::Initialize()
    {
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(DoremiEngine::Core::EngineModuleEnum::ALL);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/
        Core::PlayerHandler::StartPlayerHandler(sharedContext);
        Core::AudioHandler::StartAudioHandler(sharedContext);
        Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();


        ////////////////Example only////////////////
        // Create manager
        Core::Manager* t_renderManager = new Core::GraphicManager(sharedContext);
        // Manager* t_physicsManager = new ExampleManager(sharedContext);
        // Manager* t_playerManager = new PlayerManager(sharedContext);
        Core::Manager* t_clientNetworkManager = new Core::ClientNetworkManager(sharedContext);
        Core::Manager* t_movementManager = new Core::MovementManager(sharedContext);
        Core::Manager* t_audioManager = new Core::AudioManager(sharedContext);
        Core::Manager* t_cameraManager = new Core::CameraManager(sharedContext);
        Core::Manager* t_rigidTransSyndManager = new Core::RigidTransformSyncManager(sharedContext);
        Core::Manager* t_aiPathManager = new Core::AIPathManager(sharedContext);
        Core::Manager* t_charSyncManager = new Core::CharacterControlSyncManager(sharedContext);

        // Add manager to list of managers
        m_graphicalManagers.push_back(t_renderManager);
        // m_managers.push_back(t_physicsManager);
        // m_managers.push_back(t_playerManager);
        // m_managers.push_back(t_audioManager);
        m_managers.push_back(t_clientNetworkManager);
        m_managers.push_back(t_cameraManager);
        m_managers.push_back(t_rigidTransSyndManager);
        m_managers.push_back(t_movementManager);

        m_managers.push_back(t_aiPathManager);
        m_managers.push_back(t_charSyncManager);

        // GenerateWorld(sharedContext);

        // GenerateWorldClientJawsTest(sharedContext);
        Core::TemplateCreator::GetInstance()->CreateTemplatesForClient(sharedContext);
        SpawnDebugWorld(sharedContext);

        // Remove later, needed to see something when we play solo cause of camera interactions with input
        Doremi::Core::InputHandlerClient* inputHandler = new Doremi::Core::InputHandlerClient(sharedContext);
        Core::PlayerHandler::GetInstance()->CreateNewPlayer(300, (Doremi::Core::InputHandler*)inputHandler);
    }

    void GameMain::SpawnDebugWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();

        // Create entity
        int playerID = t_entityHandler.CreateEntity(Blueprints::PlayerEntity);

        // Create the rigid body
        int materialID = t_entityHandler.GetComponentFromStorage<Core::PhysicsMaterialComponent>(playerID)->p_materialID;
        DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 10, 0);
        DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
        Core::RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<Core::RigidBodyComponent>(playerID);
        bodyComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(playerID, position, orientation,
                                                                                                      DirectX::XMFLOAT3(0.5, 0.5, 0.5), materialID);

        sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(playerID, materialID, position, XMFLOAT2(1, 1));
        Core::EntityHandler::GetInstance().AddComponent(playerID, (int)ComponentType::CharacterController);

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
            rigidComp->p_bodyID =
                sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation, XMFLOAT3(2, 0.05, 2), matID);
        }
    }

    void GameMain::Run()
    {
        // TODOCM remove for better timer
        // GameLoop is not currently set

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
                UpdatetGame(UpdateStepLen);

                // Update interpolation transforms from snapshots
                Core::InterpolationHandler::GetInstance()->UpdateInterpolationTransforms();

                // Remove time from accumulator
                // Accumulator -= UpdateTimeStepLength;
                Accum -= UpdateStepLen;

                // Add time to start
                GameTime += UpdateStepLen;
            }

            // Update alpha usd for inteprolation
            double alpha = Accum / UpdateStepLen;

            // Interpolate the frames here
            Core::InterpolationHandler::GetInstance()->InterpolateFrame(alpha);

            // Draw stuff
            DrawGame((double)UpdateStepLen / 1000.0f);
        }
    }

    void GameMain::UpdatetGame(double p_deltaTime)
    {
        Core::EventHandler::GetInstance()->DeliverEvents();
        Core::PlayerHandler::GetInstance()->UpdatePlayerInputs();
        Core::PlayerHandler::GetInstance()->UpdatePlayerPositions();
        Core::PlayerHandler::GetInstance()->UpdatePlayerRotationsClient();
        // AudioHandler::GetInstance()->Update();

        // Have all managers update
        size_t length = m_managers.size();
        for(size_t i = 0; i < length; i++)
        {
            m_managers.at(i)->Update(p_deltaTime);
        }
    }

    void GameMain::DrawGame(double p_deltaTime)
    {
        size_t length = m_graphicalManagers.size();
        for(size_t i = 0; i < length; i++)
        {
            m_graphicalManagers.at(i)->Update(p_deltaTime);
        }
    }

    void GameMain::Start()
    {
        // Start the game
        /* Core::GameCore* gameCore = new Core::GameCore();
         gameCore->InitializeClient();
         gameCore->StartClientCore();*/


        Initialize();
        Run();
    }
}