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
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
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
#include <Doremi/Core/Include/AddRemoveSyncHandler.hpp>
#include <Doremi/Core/Include/CameraHandler.hpp>
#include <Doremi/Core/Include/PositionCorrectionHandler.hpp>
#include <Doremi/Core/Include/Handler/StateHandler.hpp>

// Managers
#include <Doremi/Core/Include/Manager/GraphicManager.hpp>
#include <Doremi/Core/Include/Manager/Network/ClientNetworkManager.hpp>
#include <Doremi/Core/Include/Manager/MovementManagerClient.hpp>
#include <Doremi/Core/Include/Manager/AudioManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AIPathManager.hpp>
#include <Doremi/Core/Include/Manager/CharacterControlSyncManager.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <Doremi/Core/Include/Manager/JumpManager.hpp>
#include <Doremi/Core/Include/Manager/SkyBoxManager.hpp>
#include <Doremi/Core/Include/Manager/GravityManager.hpp>
#include <Doremi/Core/Include/Manager/PressureParticleManager.hpp>
#include <Doremi/Core/Include/Manager/LightManager.hpp>
#include <Doremi/Core/Include/Manager/TriggerManager.hpp>
// Components
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PotentialFieldComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PlatformPatrolComponent.hpp>
// Events
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
// Other stuff
#include <Doremi/Core/Include/TemplateCreator.hpp>
#include <Doremi/Core/Include/LevelLoaderClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityFactory.hpp>
#include <Doremi/Core/Include/ScreenSpaceDrawer.hpp>


// Timer
#include <DoremiEngine\Timing\Include\Measure\MeasureTimer.hpp>

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
        TIME_FUNCTION_START
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(DoremiEngine::Core::EngineModuleEnum::ALL);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/
        Core::EntityHandler::StartupEntityHandler();
        Core::PlayerHandler::StartPlayerHandler(sharedContext);
        Core::InterpolationHandler::StartInterpolationHandler(sharedContext);
        Core::AudioHandler::StartAudioHandler(sharedContext);
        Core::StateHandler::StartStateHandler(sharedContext);
        Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();
        Core::CameraHandler::StartCameraHandler(sharedContext);
        Core::PositionCorrectionHandler::StartPositionCorrectionHandler(sharedContext);
        // Initialize 2d drawer class
        m_screenSpaceDrawer = new Core::ScreenSpaceDrawer(sharedContext, XMFLOAT2(800, 800));

        ////////////////Example only////////////////
        // Create manager
        Core::Manager* t_renderManager = new Core::GraphicManager(sharedContext);
        // Manager* t_physicsManager = new ExampleManager(sharedContext);
        // Manager* t_playerManager = new PlayerManager(sharedContext);
        Core::Manager* t_clientNetworkManager = new Core::ClientNetworkManager(sharedContext);
        Core::Manager* t_movementManager = new Core::MovementManagerClient(sharedContext);
        Core::Manager* t_audioManager = new Core::AudioManager(sharedContext);
        Core::Manager* t_rigidTransSyndManager = new Core::RigidTransformSyncManager(sharedContext);
        // Core::Manager* t_aiPathManager = new Core::AIPathManager(sharedContext);
        Core::Manager* t_charSyncManager = new Core::CharacterControlSyncManager(sharedContext);
        Core::Manager* t_jumpManager = new Core::JumpManager(sharedContext);
        Core::Manager* t_gravManager = new Core::GravityManager(sharedContext);
        Core::Manager* t_pressureParticleManager = new Core::PressureParticleManager(sharedContext);
        Core::Manager* t_skyBoxManager = new Core::SkyBoxManager(sharedContext);
        Core::Manager* t_lightManager = new Core::LightManager(sharedContext);

        Core::Manager* t_triggerManager = new Core::TriggerManager(sharedContext); // TODOKO should only be needed on server
        // Add manager to list of managers

        m_graphicalManagers.push_back(t_pressureParticleManager);
        m_graphicalManagers.push_back(t_renderManager);
        m_graphicalManagers.push_back(t_skyBoxManager);
        // m_managers.push_back(t_physicsManager);
        // m_managers.push_back(t_playerManager);
        m_managers.push_back(t_audioManager);
        m_managers.push_back(t_clientNetworkManager);
        m_managers.push_back(t_rigidTransSyndManager);

        m_managers.push_back(t_lightManager);
        // m_managers.push_back(t_jumpManager);
        // m_managers.push_back(t_gravManager);
        // m_managers.push_back(t_aiPathManager);
        m_managers.push_back(t_movementManager); // Must be after gravity/jump
        m_managers.push_back(t_charSyncManager); // Must be after movement
        m_managers.push_back(t_triggerManager); // TODOKO should only be needed on server

        // Initialize menu
        std::vector<string> t_textureNamesForMenuButtons;
        // Use this order when adding buttons. The order of the buttons can be view by hovering Menuhandler initialize under. Place highlighted TODOXX
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

        Core::TemplateCreator::GetInstance()->CreateTemplatesForClient(sharedContext);
        // GenerateWorldClientJawsTest(sharedContext);
        SpawnDebugWorld(sharedContext);

        // Remove later, needed to see something when we play solo cause of camera interactions with input
        // Doremi::Core::InputHandlerClient* inputHandler = new Doremi::Core::InputHandlerClient(sharedContext);
        // Core::PlayerHandler::GetInstance()->CreateNewPlayer(300, (Doremi::Core::InputHandler*)inputHandler);
        Doremi::Core::InputHandlerClient* inputHandler = new Doremi::Core::InputHandlerClient(sharedContext);

        TIME_FUNCTION_STOP
    }

    void GameMain::SpawnDebugWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        TIME_FUNCTION_START
        Core::EntityFactory& t_entityFactory = *Core::EntityFactory::GetInstance();
        Core::LevelLoaderClient t_levelLoader = LevelLoaderClient(sharedContext);

        t_levelLoader.LoadLevel("Levels/test.drm");

        // Create platforms
        for(size_t i = 0; i < 1; i++)
        {
            int entityID = t_entityFactory.CreateEntity(Blueprints::PlatformEntity, DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f), XMFLOAT4(0, 0, 0, 1));
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f);
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);

            float factor = 2.5;
            int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(entityID)->p_materialID;
            Core::RigidBodyComponent* rigidComp = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::RigidBodyComponent>(entityID);
            rigidComp->p_bodyID =
                sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation,
                                                                                         XMFLOAT3(2 * factor, 0.05 * factor, 2 * factor), matID);
            sharedContext.GetPhysicsModule().GetRigidBodyManager().SetKinematicActor(entityID, true);
            Core::PlatformPatrolComponent* t_platformPatrolComponent =
                Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PlatformPatrolComponent>(entityID);
            t_platformPatrolComponent->startPosition = position;
            t_platformPatrolComponent->endPosition = DirectX::XMFLOAT3(position.x, position.y + 140, position.z);
        }

        // Create some enemies
        for(size_t i = 0; i < 1; i++)
        {
            XMFLOAT3 position = DirectX::XMFLOAT3(-280 * (int)i + 0, 140 * (int)i + 4, -85 * (int)i + 15); //-2,6,60 -280, 150.0f, -85
            XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

            int entityID = t_entityFactory.CreateEntity(Blueprints::EnemyEntity, position, orientation);
        }

        TIME_FUNCTION_STOP
    }

    void GameMain::Run()
    {
        TIME_FUNCTION_START
        // TODOCM remove for better timer
        // GameLoop is not currently set

        std::chrono::time_point<std::chrono::high_resolution_clock> CurrentClock, PreviousClock;
        PreviousClock = std::chrono::high_resolution_clock::now();

        double Frame = 0;
        double Offset = 0;
        double Accum = 0;
        double GameTime = 0;
        double UpdateStepLen = 0.017f;
        double MaxFrameTime = 0.25f;

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
                    break;
                }
            }
        }
        TIME_FUNCTION_STOP
    }

    void GameMain::UpdateGame(double p_deltaTime)
    {
        TIME_FUNCTION_START
        size_t length = m_managers.size();
        PlayerHandler::GetInstance()->UpdateClient();
        AudioHandler::GetInstance()->Update(p_deltaTime);
        // TODORT
        // TODOLOG
        // Utility::Timer::MeasureTimer& timer = Utility::Timer::MeasureTimer::GetInstance();
        // Have all managers update
        for(size_t i = 0; i < length; i++)
        {
            // TODORT
            // TODOLOG
            // Utility::Timer::MeasureInfo& info = timer.GetTimer(m_managers.at(i)->GetName());
            // info.Reset().Start();
            m_managers.at(i)->Update(p_deltaTime);
            // info.Stop();
        }


        // Update camera after we update positions
        CameraHandler::GetInstance()->Update(p_deltaTime);

        PlayerHandler::GetInstance()->UpdateAddRemoveObjects();
        TIME_FUNCTION_STOP
    }

    void GameMain::UpdateMenu(double p_deltaTime) {}
    void GameMain::Update(double p_deltaTime)
    {
        TIME_FUNCTION_START
        Core::PlayerHandler::GetInstance()->UpdatePlayerInputsClient();
        Core::EventHandler::GetInstance()->DeliverEvents();
        Core::DoremiStates t_state = Core::StateHandler::GetInstance()->GetState();

        switch(t_state)
        {
            case Core::DoremiStates::MAINMENU:
                // Update Menu Logic
                MenuHandler::GetInstance()->Update(p_deltaTime);
                break;
            case Core::DoremiStates::RUNGAME:
                // Update Game logic
                UpdateGame(p_deltaTime);
                break;
            case Core::DoremiStates::EXIT:
            {
                std::cout << "You clicked exit its not ver effective state changed back to mainmenu" << std::endl;
                // State is changed with events TODOXX should be removed from here once EXIT is implemented
                Core::ChangeMenuState* menuEvent = new Core::ChangeMenuState();
                menuEvent->state = Core::DoremiStates::MAINMENU;
                Core::EventHandler::GetInstance()->BroadcastEvent(menuEvent);
                return;
            }
            break;
            case Core::DoremiStates::PAUSE:
                // Update Pause Screen
                break;
            case Core::DoremiStates::OPTIONS:
            {
                std::cout << "You clicked options button. It has no effect. State changed back to MAINMENU" << std::endl;
                // State is changed with events TODOXX should be removed from here once options is implemented
                Core::ChangeMenuState* menuEvent = new Core::ChangeMenuState();
                menuEvent->state = Core::DoremiStates::MAINMENU;
                Core::EventHandler::GetInstance()->BroadcastEvent(menuEvent);
            }
            // Update Options
            default:
                break;
        }
        TIME_FUNCTION_STOP
    }

    void GameMain::DrawMenu(double p_deltaTime)
    {
        TIME_FUNCTION_START
        MenuHandler* t_menuHandler = MenuHandler::GetInstance();
        MenuGraphicHandler::GetInstance()->DrawButtons(p_deltaTime, t_menuHandler->GetButtons(), t_menuHandler->GetCurrentButton());
        TIME_FUNCTION_STOP
    }

    void GameMain::DrawGame(double p_deltaTime)
    {
        TIME_FUNCTION_START
        size_t length = m_graphicalManagers.size();
        for(size_t i = 0; i < length; i++)
        {
            m_graphicalManagers.at(i)->Update(p_deltaTime);
        }
        TIME_FUNCTION_STOP
    }

    void GameMain::Draw(double p_deltaTime)
    {
        TIME_FUNCTION_START
        /** TODOLH Detta ska flyttas till en function som i updaten*/
        Core::DoremiStates t_state = Core::StateHandler::GetInstance()->GetState();
        switch(t_state)
        {
            case Core::DoremiStates::RUNGAME:
                // Draw Game
                DrawGame(p_deltaTime);
                break;
            default:
                break;
        }
        // WE always draw 2d stuff
        m_screenSpaceDrawer->Draw();
        TIME_FUNCTION_STOP
    }

    void GameMain::Start()
    {
        TIME_FUNCTION_START
        Initialize();
        Run();
        TIME_FUNCTION_STOP
    }
}