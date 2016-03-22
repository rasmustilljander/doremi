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

// Inputmodule
#include <DoremiEngine/Input/Include/InputModule.hpp>

/// Game
// handlers
#include <Doremi/Core/Include/InterpolationHandler.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandlerClient.hpp>
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/MenuClasses/MainMenuHandler.hpp>
#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/CameraHandler.hpp>
#include <Doremi/Core/Include/PositionCorrectionHandler.hpp>
#include <Doremi/Core/Include/Handler/StateHandler.hpp>
#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/ServerBrowserHandler.hpp>
#include <Doremi/Core/Include/SkeletalInformationHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/HUDHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/LoadingScreenHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/VictoryScreen.hpp>
#include <Doremi/Core/Include/MenuClasses/OptionsHandler.hpp>
#include <Doremi/Core/Include/Handler/TreeHandler.hpp>

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
#include <Doremi/Core/Include/SkyBoxHandler.hpp>
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

// Logger
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>

// Timer
#include <Doremi/Core/Include/Timing/TimerManager.hpp>
#include <Doremi/Core/Include/Timing/NamedTimer.hpp>
#include <Doremi/Core/Include/Timing/FunctionTimer.hpp>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>

#include <Utility/Utilities/Include/Test/VariableManager.hpp>

// Third party

// Standard libraries
#include <exception>
#include <chrono>
#include <vector>
#include <iostream> //TODOLH remove once all the functionality is implemented in the menusystem
#include <Windows.h>

namespace Doremi
{
    using namespace Utilities::Logging;
    using namespace Core;
    GameMain::GameMain() : m_sharedContext(nullptr), m_gameRunning(true)
    {
        VariableManager& v = VariableManager::GetVariablesManager();
        m_messagesPerUpdate = v.GetValue<int>("messagesPerUpdate");
        m_testLength = v.GetValue<float>("testLength");
        m_stopAfterInitialize = v.GetValue<bool>("stopAfterInitialize");
        m_message = v.GetValue<std::string>("message");
        std::cout << m_testLength << std::endl;
    }

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
        FUNCTION_TIMER

        using namespace Core;
        const DoremiEngine::Core::SharedContext& sharedContext = InitializeEngine(DoremiEngine::Core::EngineModuleEnum::ALL);
        m_sharedContext = &sharedContext;

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
        SkyBoxHandler::StartupSkyBoxHandler(sharedContext);
        ServerBrowserHandler::StartupServerBrowserHandler(sharedContext);
        SkeletalInformationHandler::StartSkeletalInformationHandler(sharedContext);
        HUDHandler::StartHUDHandler(sharedContext);
        LoadingScreenHandler::StartLoadingScreenHandler(sharedContext);
        ScreenSpaceDrawer::StartupScreenSpaceDrawer(sharedContext);
        VictoryScreen::StartupVictoryScreen(sharedContext);
        OptionsHandler::StartOptionsHandler(sharedContext);
        Core::TreeHandler::StartupTreeHandler(sharedContext);

        // Initialize 2d drawer class
        m_screenRes = m_sharedContext->GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetScreenResolution();
        m_screenSpaceDrawer = ScreenSpaceDrawer::GetInstance();

        // Create manager & add manager to list of managers
        AddToGraphicalManagerList(new PressureParticleGraphicManager(sharedContext));
        AddToGraphicalManagerList(new GroundEffectManagerClient(sharedContext));
        AddToGraphicalManagerList(new GraphicManager(sharedContext));
        AddToGraphicalManagerList(new SkeletalAnimationCoreManager(sharedContext));
        AddToManagerList(new AudioManager(sharedContext));
        NetworkManagerClient* t_netManager = new NetworkManagerClient(sharedContext);
        AddToManagerList(t_netManager);
        AddToServerBrowserList(t_netManager);
        AddToManagerList(new RigidTransformSyncManager(sharedContext));
        AddToManagerList(new PressureParticleManager(sharedContext));
        AddToManagerList(new LightManager(sharedContext));
        AddToManagerList(new JumpManager(sharedContext));
        AddToManagerList(new GravityManager(sharedContext));
        AddToManagerList(new MovementManagerClient(sharedContext)); // Must be after gravity/jump
        AddToManagerList(new CharacterControlSyncManager(sharedContext)); // Must be after movement
        AddToManagerList(new TriggerManager(sharedContext)); // TODOKO should only be needed on server
        AddToManagerList(new ExtraDrainSyncManager(sharedContext));

        MainMenuHandler::StartMainMenuHandler(sharedContext, m_screenRes);
        MainMenuHandler::GetInstance()->Initialize();

        TemplateCreator::GetInstance()->CreateTemplatesForClient(sharedContext);
        // BuildWorld(sharedContext);


        AudioHandler::GetInstance()->SetupContinuousRecording();
        AudioHandler::GetInstance()->StartContinuousRecording();
        AudioHandler::GetInstance()->SetupRepeatableRecording();
    }

    void GameMain::AddToManagerList(Manager* p_manager) { m_managers.push_back(p_manager); }

    void GameMain::AddToServerBrowserList(Manager* p_manager) { m_serverBrowserManagers.push_back(p_manager); }

    void GameMain::AddToGraphicalManagerList(Manager* p_manager) { m_graphicalManagers.push_back(p_manager); }

    void GameMain::BuildWorld(const DoremiEngine::Core::SharedContext& sharedContext)
    {
        // FUNCTION_TIME
        // Core::EntityFactory& t_entityFactory = *Core::EntityFactory::GetInstance();
        // Core::LevelLoaderClient t_levelLoader(sharedContext);



        // t_levelLoader.LoadLevel("Levels/IntroScene.drm");
        // t_levelLoader.LoadLevel("Levels/TestSceneCulling.drm");


        ////// Create platforms
        ////for(size_t i = 0; i < 1; i++)
        ////{
        ////    int entityID = t_entityFactory.CreateEntity(Blueprints::PlatformEntity, DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f), XMFLOAT4(0, 0, 0,
        /// 1));
        ////    DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(-165.75f, 4.6f, -103.74f); // why not pass as parameter in above method?
        ////    DirectX::XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);

        ////    Core::PlatformPatrolComponent* t_platformPatrolComponent =
        ////        Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PlatformPatrolComponent>(entityID);
        ////    t_platformPatrolComponent->startPosition = position;
        ////    t_platformPatrolComponent->endPosition = DirectX::XMFLOAT3(position.x, position.y + 140, position.z);
        ////}

        //// Create an enemy spawner (only necessary to keep entityIDs aligned with server)
        // TIME_FUNCTION_STOP
    }

    void GameMain::Run()
    {
        FUNCTION_TIMER
        // TODOCM remove for better timer
        // GameLoop is not currently set
        TimeHandler* t_timeHandler = TimeHandler::GetInstance();

        t_timeHandler->PreviousClock = std::chrono::high_resolution_clock::now();
        while(m_gameRunning)
        {
            // Tick time
            t_timeHandler->Tick();

            // Loop as many update-steps we will take this frame
            while(m_gameRunning && t_timeHandler->ShouldUpdateFrame())
            {
                // Update game based on state
                Update(t_timeHandler->UpdateStepLen);

                // Update interpolation transforms from snapshots
                Core::InterpolationHandler::GetInstance()->UpdateInterpolationTransforms();

                // Deliver events
                static_cast<Core::EventHandlerClient*>(Core::EventHandler::GetInstance())->DeliverEvents();

                // Update accumulator and gametime
                t_timeHandler->UpdateAccumulatorAndGameTime();

                for(int i = 0; i < m_messagesPerUpdate; ++i)
                {
                    m_logger->LogText(LogTag::GENERAL, LogLevel::MASS_DATA_PRINT, "%s", m_message.c_str());
                }
            }

            // Update alpha usd for inteprolation
            double alpha = t_timeHandler->GetFrameAlpha();

            if(t_timeHandler->GameTime > m_testLength && m_testLength != 0.0)
            {
                m_gameRunning = false;
            }

            // Interpolate the frames here
            Core::InterpolationHandler::GetInstance()->InterpolateFrame(alpha);

            // Update camera after we update positions
            CameraHandler::GetInstance()->UpdateDraw(alpha);
            Draw(t_timeHandler->Frame);
        }
    }

    void GameMain::UpdateGame(double p_deltaTime)
    {
        FUNCTION_TIMER

        PlayerHandler::GetInstance()->Update(p_deltaTime);
        // AudioHandler::GetInstance()->Update(p_deltaTime);
        // Have all managers update
        const size_t length = m_managers.size();
        for(size_t i = 0; i < length; i++)
        {
            const std::string& name = m_managers.at(i)->GetName();
            NAMED_TIMER(name);
            m_managers.at(i)->Update(p_deltaTime);
        }

        CameraHandler::GetInstance()->UpdateInput(p_deltaTime);
    }

    void GameMain::UpdateServerBrowser(double p_deltaTime)
    {
        FUNCTION_TIMER

        PlayerHandler::GetInstance()->Update(p_deltaTime);

        // Have all managers update
        const size_t length = m_serverBrowserManagers.size();
        using namespace Utilities::Logging;
        for(size_t i = 0; i < length; i++)
        {
            const std::string& name = m_serverBrowserManagers.at(i)->GetName();
            NAMED_TIMER(name);
            m_serverBrowserManagers.at(i)->Update(p_deltaTime);
        }
    }

    void GameMain::Update(double p_deltaTime)
    {
        FUNCTION_TIMER
        static_cast<PlayerHandlerClient*>(Core::PlayerHandler::GetInstance())->UpdatePlayerInputs();

        AudioHandler::GetInstance()->Update(p_deltaTime);

        Core::DoremiGameStates t_state = Core::StateHandler::GetInstance()->GetState();

        switch(t_state)
        {
            case Core::DoremiGameStates::MAINMENU:
            {
                // Update Menu Logic
                MainMenuHandler::GetInstance()->Update(p_deltaTime);
                break;
            }
            case Core::DoremiGameStates::SERVER_BROWSER:
            {
                // TODOCM maybe only run network manager
                UpdateServerBrowser(p_deltaTime);
                ServerBrowserHandler::GetInstance()->Update(p_deltaTime);
                break;
            }
            case Core::DoremiGameStates::OPTIONS:
            {
                OptionsHandler::GetInstance()->Update(p_deltaTime);

                break;
            }
            case Core::DoremiGameStates::LOADING:
            {
                UpdateGame(p_deltaTime);
                LoadingScreenHandler::GetInstance()->Update(p_deltaTime);
                break;
            }
            case Core::DoremiGameStates::RUNGAME:
            {
                // Update Game logic
                UpdateGame(p_deltaTime);
                HUDHandler::GetInstance()->Update(p_deltaTime);
                break;
            }
            case Core::DoremiGameStates::PAUSE:
            {
                // Update Pause Screen
                break;
            }

            case Core::DoremiGameStates::EXIT:
            {
                m_gameRunning = false;
                break;
            }
            case Core::DoremiGameStates::VICTORY:
            {
                VictoryScreen::GetInstance()->Update();
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void GameMain::DrawGame(double p_deltaTime)
    {
        using namespace Utilities::Logging;

        FUNCTION_TIMER
        const size_t length = m_graphicalManagers.size();
        using namespace Utilities::Logging;
        for(size_t i = 0; i < length; i++)
        {
            const std::string& name = m_graphicalManagers.at(i)->GetName();
            NAMED_TIMER(name);
            m_graphicalManagers.at(i)->Update(p_deltaTime);
        }
        SkyBoxHandler::GetInstance()->Draw();
    }

    void GameMain::Draw(double p_deltaTime)
    {
        FUNCTION_TIMER

        {
            NAMED_TIMER("BeginRaw")
            m_sharedContext->GetGraphicModule().GetSubModuleManager().GetDirectXManager().BeginDraw();
        }

        /** TODOLH Detta ska flyttas till en function som i updaten*/
        Core::DoremiGameStates t_state = Core::StateHandler::GetInstance()->GetState();
        switch(t_state)
        {
            case Core::DoremiGameStates::RUNGAME:
                // Draw Game
                DrawGame(p_deltaTime);
                break;
            default:
                break;
        }

        {
            // WE always draw 2d stuff
            NAMED_TIMER("m_screenSpaceDrawer->Draw")
            m_screenSpaceDrawer->Draw();
        }
        {
            NAMED_TIMER("EndRaw")
            m_sharedContext->GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
        }
    }

    void GameMain::Start()
    {
        try
        {
            FUNCTION_TIMER
            Initialize();
            if(!m_stopAfterInitialize)
            {
                m_logger->LogText(LogTag::GAME, LogLevel::INFO, "Starting");
                Run();
            }

            auto& map = VariableManager::GetVariablesManager().GetAllValues();
            m_logger->LogText(LogTag::GAME, LogLevel::INFO, "Simulation parameters");

            for(auto& i : map)
            {
                WrappedType type = ((WrapperContainer<void*>*)i.second)->type;
                if(type == WrappedType::INT)
                {
                    m_logger->LogText(LogTag::TIMER, LogLevel::MASS_DATA_PRINT, "%s: %d", i.first.c_str(), *((WrapperContainer<int>*)i.second)->data);
                }
                else if(type == WrappedType::STRING)
                {
                    m_logger->LogText(LogTag::TIMER, LogLevel::MASS_DATA_PRINT, "%s: %s", i.first.c_str(),
                                      (*((WrapperContainer<std::string>*)i.second)->data).c_str());
                }
                else if(type == WrappedType::FLOAT)
                {
                    m_logger->LogText(LogTag::TIMER, LogLevel::MASS_DATA_PRINT, "%s: %f", i.first.c_str(), *((WrapperContainer<float>*)i.second)->data);
                }
                else if(type == WrappedType::BOOL)
                {
                    m_logger->LogText(LogTag::TIMER, LogLevel::MASS_DATA_PRINT, "%s: %d", i.first.c_str(), *((WrapperContainer<bool>*)i.second)->data);
                }
            }
        }
        catch(const std::exception& exception)
        {
            using namespace Utilities::Logging;
            m_logger->LogText(LogTag::GAME, LogLevel::FATAL_ERROR, "Exception: %s", exception.what());
        }
        Doremi::Core::TimerManager::GetInstance().DumpData(*m_sharedContext);
    }

    void GameMain::Stop()
    {
        Core::ChangeMenuState* menuEvent = new Core::ChangeMenuState();
        menuEvent->state = Core::DoremiGameStates::EXIT;
        Core::EventHandler::GetInstance()->BroadcastEvent(menuEvent);
    }
}