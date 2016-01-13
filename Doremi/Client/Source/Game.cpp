// Project specific
#include <Game.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
/// Engine
// Core
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Physics
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>

/// Game
// handlers
#include <Doremi/Core/Include/InterpolationHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/MenuClasses/MenuHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/MenuGraphicHandler.hpp>
// Managers
#include <Doremi/Core/Include/Manager/GraphicManager.hpp>
#include <Doremi/Core/Include/Manager/Network/ClientNetworkManager.hpp>
#include <Doremi/Core/Include/Manager/MovementManager.hpp>
#include <Doremi/Core/Include/Manager/AudioManager.hpp>
#include <Doremi/Core/Include/Manager/CameraManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AIPathManager.hpp>
#include <Doremi/Core/Include/Manager/CharacterControlSyncManager.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <Doremi/Core/Include/Manager/JumpManager.hpp>
#include <Doremi/Core/Include/Manager/GravityManager.hpp>
// Components
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PotentialFieldComponent.hpp>
// Other stuff
#include <Doremi/Core/Include/TemplateCreator.hpp>
#include <Doremi/Core/Include/LevelLoader.hpp>

// Timer
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

// Third party

// Standard libraries
#include <exception>
#include <chrono>
#include <vector>
#include <iostream> //TODOLH remove once all the functionality is implemented in the menusystem

namespace Doremi
{
    namespace Core
    {
        class Manager;
        class EntityInterface;
    }
}

namespace Doremi
{
    using namespace Core;
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
        Core::Manager* t_jumpManager = new Core::JumpManager(sharedContext);
        Core::Manager* t_gravManager = new Core::GravityManager(sharedContext);

        // Add manager to list of managers
        m_graphicalManagers.push_back(t_renderManager);
        // m_managers.push_back(t_physicsManager);
        // m_managers.push_back(t_playerManager);
        // m_managers.push_back(t_audioManager);
        m_managers.push_back(t_clientNetworkManager);
        m_managers.push_back(t_cameraManager);
        // m_managers.push_back(t_rigidTransSyndManager);
        m_managers.push_back(t_movementManager);
        m_managers.push_back(t_jumpManager);
        m_managers.push_back(t_gravManager);

        // m_managers.push_back(t_aiPathManager);
        m_managers.push_back(t_charSyncManager);
        // Initialize menu
        std::vector<string> t_textureNamesForMenuButtons;
        // Use this order when adding buttons. The order of the buttons can be view by hovering Menuhandler initialize under. Place highlighted
        // textures under in the same order
        t_textureNamesForMenuButtons.push_back("Playbutton.dds");
        t_textureNamesForMenuButtons.push_back("Optionsbutton.dds");
        t_textureNamesForMenuButtons.push_back("Exitbutton.dds");
        t_textureNamesForMenuButtons.push_back("PlaybuttonHighlighted.dds");
        t_textureNamesForMenuButtons.push_back("OptionsbuttonHighlighted.dds");
        t_textureNamesForMenuButtons.push_back("ExitbuttonHighlighted.dds");
        MenuHandler::StartMenuHandler(sharedContext, DirectX::XMFLOAT2(800.0f, 800.0f));
        MenuHandler::GetInstance()->Initialize(t_textureNamesForMenuButtons);
        // initialize menudraw
        MenuGraphicHandler::StartMenuGraphicHandler(sharedContext);


        // GenerateWorld(sharedContext);

        // GenerateWorldClientJawsTest(sharedContext);
        Core::TemplateCreator::GetInstance()->CreateTemplatesForClient(sharedContext);
        SpawnDebugWorld(sharedContext);

        // Remove later, needed to see something when we play solo cause of camera interactions with input
        // Doremi::Core::InputHandlerClient* inputHandler = new Doremi::Core::InputHandlerClient(sharedContext);
        // Core::PlayerHandler::GetInstance()->CreateNewPlayer(300, (Doremi::Core::InputHandler*)inputHandler);
        Doremi::Core::InputHandlerClient* inputHandler = new Doremi::Core::InputHandlerClient(sharedContext);
        m_menuState = MenuStates::MenuState::RUNGAME; // byt denna till MAINMENU om du vill se menyn!! TODOLH
    }

    void GameMain::SpawnDebugWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();
        Core::LevelLoader* t_levelLoader = new Core::LevelLoader(sharedContext);
        t_levelLoader->LoadLevel("Levels/test.drm");
        // Create Avatar entity
        /*  int playerID = t_entityHandler.CreateEntity(Blueprints::PlayerEntity);
          int materialID = t_entityHandler.GetComponentFromStorage<Core::PhysicsMaterialComponent>(playerID)->p_materialID;
          DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 10, 0);
          DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
          sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(playerID, materialID, position, XMFLOAT2(1, 1));*/

        int entityDebugJaws = t_entityHandler.CreateEntity(Blueprints::JawsDebugEntity);
        Core::TransformComponent* trans = GetComponent<Core::TransformComponent>(entityDebugJaws);
        trans->position = DirectX::XMFLOAT3(-10.0f, 5.0f, 0.0f);

        // Create platforms
        for(size_t i = 0; i < 5; i++)
        {
            int entityID = t_entityHandler.CreateEntity(Blueprints::PlatformEntity);
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 10.0f - (float)i, i * 5.0f);
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            Core::RigidBodyComponent* rigidComp = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::RigidBodyComponent>(entityID);
            rigidComp->p_bodyID =
                sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(entityID, position, orientation, XMFLOAT3(2, 0.05, 2), matID);
        }

        // Create some enemies
        for(size_t i = 0; i < 8; i++)
        {
            int entityID = t_entityHandler.CreateEntity(Blueprints::EnemyEntity);
            XMFLOAT3 position = DirectX::XMFLOAT3(0, 7 - (int)i, i * 5);
            XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
            // int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(entityID)->p_materialID;
            // RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
            // rigidComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation,
            //                                                                                               XMFLOAT3(0.5, 0.5, 0.5), matID);
            // sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(entityID, matID, position, XMFLOAT2(0.1, 0.5));

            // PotentialFieldComponent* potentialComponent = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(entityID);
            // potentialComponent->ChargedActor = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(DirectX::XMFLOAT3(0, 0, 0),
            // -1, 3);

        }

        /////// TONS OF OLD CODE//////
        // Create the rigid body
        // Core::RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<Core::RigidBodyComponent>(playerID);
        // bodyComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(playerID, position, orientation,
        //                                                                                              DirectX::XMFLOAT3(0.5, 0.5, 0.5), materialID);


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
                Update(UpdateStepLen);
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
            Draw(UpdateStepLen);
            // Escape
            InputHandlerClient* inputHandler = (InputHandlerClient*)PlayerHandler::GetInstance()->GetDefaultInputHandler();
            if(inputHandler != nullptr)
            {
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::ExitGame))
                {
                    return;
                }
            }
        }
    }

    void GameMain::UpdateGame(double p_deltaTime)
    {
        size_t length = m_managers.size();
        Core::EventHandler::GetInstance()->DeliverEvents();
        PlayerHandler::GetInstance()->UpdateClient();
        AudioHandler::GetInstance()->Update(p_deltaTime);

        Utility::Timer::MeasureTimer& timer = Utility::Timer::MeasureTimer::GetInstance();
        // Have all managers update
        for(size_t i = 0; i < length; i++)
        {
            Utility::Timer::MeasureInfo& info = timer.GetTimer(m_managers.at(i)->GetName());
            info.Reset().Start();
            m_managers.at(i)->Update(p_deltaTime);
            info.Stop();
        }
    }

    void GameMain::UpdateMenu(double p_deltaTime) { m_menuState = (MenuStates::MenuState)MenuHandler::GetInstance()->Update(p_deltaTime); }
    void GameMain::Update(double p_deltaTime)
    {
        Core::PlayerHandler::GetInstance()->UpdatePlayerInputs();
        switch(m_menuState)
        {
            case MenuStates::MAINMENU:
                // Update Menu Logic
                UpdateMenu(p_deltaTime);
                break;
            case MenuStates::RUNGAME:
                // Update Game logic
                UpdateGame(p_deltaTime);
                break;
            case MenuStates::EXIT:
                std::cout << "You clicked exit its not ver effective state changed back to mainmenu" << std::endl;
                m_menuState = MenuStates::MAINMENU;
                return;
                break;
            case MenuStates::PAUSE:
                // Update Pause Screen
                break;
            case MenuStates::OPTIONS:
                std::cout << "You clicked options button. It has no effect. State changed back to MAINMENU" << std::endl;
                m_menuState = MenuStates::MAINMENU;
            // Update Options
            default:
                break;
        }
    }

    void GameMain::DrawMenu(double p_deltaTime)
    {
        MenuHandler* t_menuHandler = MenuHandler::GetInstance();
        MenuGraphicHandler::GetInstance()->DrawButtons(p_deltaTime, t_menuHandler->GetButtons(), t_menuHandler->GetCurrentButton());
    }

    void GameMain::DrawGame(double p_deltaTime)
    {
        size_t length = m_graphicalManagers.size();
        for(size_t i = 0; i < length; i++)
        {
            m_graphicalManagers.at(i)->Update(p_deltaTime);
        }
    }

    void GameMain::Draw(double p_deltaTime)
    {
        /** TODOLH Detta ska flyttas till en function som i updaten*/
        switch(m_menuState)
        {
            case MenuStates::MAINMENU:
                // Draw mainMenu
                DrawMenu(p_deltaTime);
                break;
            case MenuStates::RUNGAME:
                // Draw Game
                DrawGame(p_deltaTime);
                break;
            case MenuStates::PAUSE:
                // Draw PauseSCreen
                break;
            default:
                break;
        }
    }

    void GameMain::Start()
    {
        Initialize();
        Run();
    }
}