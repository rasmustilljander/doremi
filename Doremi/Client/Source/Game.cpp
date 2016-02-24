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
// GRAPHICS
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>

/// Game
// handlers
#include <Doremi/Core/Include/InterpolationHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandlerClient.hpp>
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/MenuClasses/MenuHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/MenuGraphicHandler.hpp>
#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/CameraHandler.hpp>
#include <Doremi/Core/Include/PositionCorrectionHandler.hpp>
#include <Doremi/Core/Include/Handler/StateHandler.hpp>
#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>

// Managers
#include <Doremi/Core/Include/Manager/GraphicManager.hpp>
#include <Doremi/Core/Include/Manager/SkeletalAnimationCoreManager.hpp>
#include <Doremi/Core/Include/Network/NetworkManagerClient.hpp>
#include <Doremi/Core/Include/Manager/MovementManagerClient.hpp>
#include <Doremi/Core/Include/Manager/AudioManager.hpp>
#include <Doremi/Core/Include/Manager/AI/AIPathManager.hpp>
#include <Doremi/Core/Include/Manager/CharacterControlSyncManager.hpp>
#include <Doremi/Core/Include/Manager/RigidTransformSyncManager.hpp>
#include <Doremi/Core/Include/Manager/JumpManager.hpp>
#include <Doremi/Core/Include/Manager/SkyBoxManager.hpp>
#include <Doremi/Core/Include/Manager/GravityManager.hpp>
#include <Doremi/Core/Include/Manager/PressureParticleGraphicManager.hpp>
#include <Doremi/Core/Include/Manager/PressureParticleManager.hpp>
#include <Doremi/Core/Include/Manager/LightManager.hpp>
#include <Doremi/Core/Include/Manager/TriggerManager.hpp>
#include <Doremi/Core/Include/Manager/ExtraDrainSyncManager.hpp>
#include <Doremi/Core/Include/Manager/GroundEffectManagerClient.hpp>
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
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

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

    GameMain::~GameMain()
    {
        for(auto& manager : m_managers)
        {
            delete manager;
        }
        m_managers.clear();

        for(auto& manager : m_graphicalManagers)
        {
            delete manager;
        }
        m_graphicalManagers.clear();
    }

    void GameMain::Initialize()
    {
        TIME_FUNCTION_START

        using namespace Core;
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(DoremiEngine::Core::EngineModuleEnum::ALL);

        /* This starts the physics handler. Should not be done here, but since this is the general
        code dump, it'll work for now TODOJB*/
        EventHandlerClient::StartupEventHandlerClient();
        EntityHandlerClient::StartupEntityHandlerClient(sharedContext);
        PlayerHandlerClient::StartPlayerHandlerClient(sharedContext);
        InterpolationHandler::StartInterpolationHandler(sharedContext);
        AudioHandler::StartAudioHandler(sharedContext); // Needs to be stareted after event handler
        StateHandler::StartStateHandler(sharedContext);
        CameraHandler::StartCameraHandler(sharedContext);
        PositionCorrectionHandler::StartPositionCorrectionHandler(sharedContext);
        EntityFactory::StartupEntityFactory(sharedContext);
        PlayerSpawnerHandler::StartupPlayerSpawnerHandler(sharedContext);

        // Initialize 2d drawer class
        m_screenRes = m_sharedContext->GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetScreenResolution();
        m_screenSpaceDrawer = new ScreenSpaceDrawer(sharedContext, m_screenRes);

        // Create manager & add manager to list of managers
        AddToGraphicalManagerList(new PressureParticleGraphicManager(sharedContext));
        AddToGraphicalManagerList(new GraphicManager(sharedContext));
        AddToGraphicalManagerList(new SkeletalAnimationCoreManager(sharedContext));
        AddToGraphicalManagerList(new SkyBoxManager(sharedContext));
        AddToManagerList(new AudioManager(sharedContext));
        AddToManagerList(new NetworkManagerClient(sharedContext));
        AddToManagerList(new RigidTransformSyncManager(sharedContext));
        AddToManagerList(new PressureParticleManager(sharedContext));
        AddToManagerList(new LightManager(sharedContext));
        AddToManagerList(new JumpManager(sharedContext));
        AddToManagerList(new GravityManager(sharedContext));
        AddToManagerList(new MovementManagerClient(sharedContext)); // Must be after gravity/jump
        AddToManagerList(new CharacterControlSyncManager(sharedContext)); // Must be after movement
        AddToManagerList(new TriggerManager(sharedContext)); // TODOKO should only be needed on server
        AddToGraphicalManagerList(new ExtraDrainSyncManager(sharedContext));
        AddToGraphicalManagerList(new GroundEffectManagerClient(sharedContext));

        // Initialize menu
        std::vector<string> t_textureNamesForMenuButtons;

        // Use this order when adding buttons. The order of the buttons can be view by hovering Menuhandler initialize under. Place highlighted TODOXX
        // textures under in the same order
        t_textureNamesForMenuButtons.push_back("playbutton2.dds");
        t_textureNamesForMenuButtons.push_back("optionsbutton2.dds");
        t_textureNamesForMenuButtons.push_back("exitbutton2.dds");
        t_textureNamesForMenuButtons.push_back("playbutton2highlight.dds");
        t_textureNamesForMenuButtons.push_back("optionsbutton2highlight.dds");
        t_textureNamesForMenuButtons.push_back("exitbutton2highlight.dds");
        MenuHandler::StartMenuHandler(sharedContext, m_screenRes);
        MenuHandler::GetInstance()->Initialize(t_textureNamesForMenuButtons);

        // initialize menudraw
        MenuGraphicHandler::StartMenuGraphicHandler(sharedContext);

        TemplateCreator::GetInstance()->CreateTemplatesForClient(sharedContext);
        BuildWorld(sharedContext);

        AudioHandler::GetInstance()->SetupContinuousRecording();
        AudioHandler::GetInstance()->StartContinuousRecording();
        AudioHandler::GetInstance()->SetupRepeatableRecording();

        TIME_FUNCTION_STOP
    }

    void GameMain::AddToManagerList(Manager* p_manager) { m_managers.push_back(p_manager); }

    void GameMain::AddToGraphicalManagerList(Manager* p_manager) { m_graphicalManagers.push_back(p_manager); }

    void GameMain::BuildWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        TIME_FUNCTION_START
        Core::EntityFactory& t_entityFactory = *Core::EntityFactory::GetInstance();
        Core::LevelLoaderClient t_levelLoader(sharedContext);

        t_levelLoader.LoadLevel("Levels/IntroScene.drm");


        // Create platforms
        for(size_t i = 0; i < 1; i++)
        {
            int entityID = t_entityFactory.CreateEntity(Blueprints::PlatformEntity, DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f), XMFLOAT4(0, 0, 0, 1));
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f); // why not pass as parameter in above method?
            DirectX::XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);

            Core::PlatformPatrolComponent* t_platformPatrolComponent =
                Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PlatformPatrolComponent>(entityID);
            t_platformPatrolComponent->startPosition = position;
            t_platformPatrolComponent->endPosition = DirectX::XMFLOAT3(position.x, position.y + 140, position.z);
        }

        // Create an enemy spawner (only necessary to keep entityIDs aligned with server)
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
        double UpdateStepLen = 0.017f * 2;
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
                // Update game based on state
                Update(UpdateStepLen);

                // Update interpolation transforms from snapshots
                Core::InterpolationHandler::GetInstance()->UpdateInterpolationTransforms();

                // Deliver events
                static_cast<Core::EventHandlerClient*>(Core::EventHandler::GetInstance())->DeliverEvents();

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

            // Update camera after we update positions
            CameraHandler::GetInstance()->UpdateDraw();

            Draw(Frame);

            // Escape
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            InputHandlerClient* inputHandler = t_playerHandler->GetInputHandler();
            if(inputHandler != nullptr)
            {
                if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::ExitGame))
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
        PlayerHandler::GetInstance()->Update(p_deltaTime);
        // AudioHandler::GetInstance()->Update(p_deltaTime);
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

        CameraHandler::GetInstance()->UpdateInput(p_deltaTime);


        // PlayerHandler::GetInstance()->UpdateAddRemoveObjects();

        TIME_FUNCTION_STOP
    }

    void GameMain::UpdateMenu(double p_deltaTime) {}

    void GameMain::Update(double p_deltaTime)
    {
        TIME_FUNCTION_START
        static_cast<PlayerHandlerClient*>(Core::PlayerHandler::GetInstance())->UpdatePlayerInputs();

        AudioHandler::GetInstance()->Update(p_deltaTime);

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
        m_sharedContext->GetGraphicModule().GetSubModuleManager().GetDirectXManager().BeginDraw();
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