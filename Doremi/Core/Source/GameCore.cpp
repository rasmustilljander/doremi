// Project specific
#include <GameCore.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <Manager/Manager.hpp>
#include <Manager/ExampleManager.hpp>
#include <Manager/AudioManager.hpp>
#include <Manager/Network/ClientNetworkManager.hpp>
#include <Manager/Network/ServerNetworkManager.hpp>
#include <Manager/GraphicManager.hpp>
#include <Manager/CameraManager.hpp>
#include <Manager/RigidTransformSyncManager.hpp>
#include <Utility/DynamicLoader/Include/DynamicLoader.hpp>
#include <DoremiEngine/Core/Include/DoremiEngine.hpp>
#include <DoremiEngine/Core/Include/Subsystem/EngineModuleEnum.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EventHandler/EventHandler.hpp>
#include <InputHandler.hpp>
#include <EntityComponent/Components/AudioActiveComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PlayerComponent.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <PlayerHandler.hpp>
#include <Manager/MovementManager.hpp>
#include <AudioHandler.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EventHandler/Events/PlayerCreationEvent.hpp>

#include <string>

// TODOCM remove for better timer?
#include <Windows.h>

// Third party

// Standard libraries
#include <iostream> // Only debugging

using namespace std;
namespace Doremi
{
    namespace Core
    {
        GameCore::GameCore() { LoadEngineLibrary(); }

        GameCore::~GameCore()
        {
            m_stopEngineFunction();
            DynamicLoader::FreeSharedLibrary(m_engineLibrary);
        }
        void CreateBulletBlueprint(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityBlueprint blueprint;
            /// Create Components
            // Transform
            TransformComponent* transComp = new TransformComponent();
            blueprint[ComponentType::Transform] = transComp;
            // Render
            RenderComponent* renderComp = new RenderComponent();
            renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            blueprint[ComponentType::Render] = renderComp;
            // PhysicsMaterialComp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.5, 0.5, 0.5);
            blueprint[ComponentType::PhysicalMaterial] = t_physMatComp;
            // Rigid body comp
            RigidBodyComponent* rigidBodyComp = new RigidBodyComponent();
            blueprint[ComponentType::RigidBody] = rigidBodyComp;

            /// Register blueprint
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::BulletEntity, blueprint);
        }
        void GenerateDebugPlatforms(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            // Simple debug platform
            EntityBlueprint t_platform;
            // Render
            RenderComponent* t_renderComp = new RenderComponent();
            t_renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            t_platform[ComponentType::Render] = t_renderComp;
            // Transform
            TransformComponent* t_transformComp = new TransformComponent();
            t_transformComp->scale.x = 4;
            t_transformComp->scale.z = 4;
            t_transformComp->scale.y = 0.1;
            t_platform[ComponentType::Transform] = t_transformComp;
            // Physical material comp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_platform[ComponentType::PhysicalMaterial] = t_physMatComp;
            // Rigid body comp
            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_platform[ComponentType::RigidBody] = t_rigidBodyComp;
            // Audio comp
            AudioComponent* t_audioComponent = new AudioComponent();
            t_audioComponent->soundID = sharedContext.GetAudioModule().\
                LoadSound("Sounds/Test sounds/1 amp som har låg frekv sen hög, Human made!372hz till 643hz.wav", 0.5f, 5000.0f);
            t_platform[ComponentType::Audio] = t_audioComponent;
            // ActiveAudiocomp
            AudioActiveComponent* t_audioActiveComponent = new AudioActiveComponent();
            AudioHandler::GetInstance()->Initialize();
            sharedContext.GetAudioModule().PlayASound(t_audioComponent->soundID, true, t_audioActiveComponent->channelID);
            sharedContext.GetAudioModule().SetVolumeOnChannel(t_audioActiveComponent->channelID, 0.0f);
            t_platform[ComponentType::AudioActive] = t_audioActiveComponent;
            size_t t_soundIDForRecord = sharedContext.GetAudioModule().SetupRecording(true);
            sharedContext.GetAudioModule().StartRecording(t_soundIDForRecord, true);
            AudioHandler::GetInstance()->SetFrequencyAnalyserSoundID(t_soundIDForRecord);
            AudioHandler::GetInstance()->SetLoopForFrequencyAnalyser(true);
            sharedContext.GetAudioModule().SetSoundPositionAndVelocity(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), 0);
            // Add label for frequency Check
            t_platform[ComponentType::FrequencyAffected];
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlatformEntity, t_platform);

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
                XMFLOAT3 position = DirectX::XMFLOAT3(0, 10 - (int)i, i * 5);
                XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
                int matID = EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(entityID)->p_materialID;
                RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
                rigidComp->p_bodyID =
                    sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(position, orientation, XMFLOAT3(2, 0.05, 2), matID);
            }
        }
        // Only for testing, should be removed! TODO
        void GenerateWorld(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            GenerateDebugPlatforms(sharedContext);
            CreateBulletBlueprint(sharedContext);
            // Create components
            ExampleComponent* t_exampleComponent = new ExampleComponent(5, 5);
            Example2Component* t_example2Component = new Example2Component();

            AudioComponent* t_audioComp = new AudioComponent(); /**TODOLH only for testing, should be removed!*/
            AudioActiveComponent* t_audioActiveComp = new AudioActiveComponent();

            RenderComponent* t_renderComp = new RenderComponent();
            TransformComponent* t_transformComp = new TransformComponent();

            EntityBlueprint t_avatarBlueprint;

            /// Fill with components
            // Render
            t_renderComp = new RenderComponent();
            t_renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            t_avatarBlueprint[ComponentType::Render] = t_renderComp;
            // Transform comp
            t_transformComp = new TransformComponent();
            t_avatarBlueprint[ComponentType::Transform] = t_transformComp;
            // Physical material comp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_avatarBlueprint[ComponentType::PhysicalMaterial] = t_physMatComp;
            // Rigid body comp
            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_avatarBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;
            // Player component
            PlayerComponent* t_playerComp = new PlayerComponent();
            t_playerComp->isControllable = true;
            t_avatarBlueprint[ComponentType::Player] = t_playerComp;
            // Movement Component
            MovementComponent* t_movementComp = new MovementComponent();
            t_movementComp->maxSpeed = 5;
            t_avatarBlueprint[ComponentType::Movement] = t_movementComp;
            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlayerEntity, t_avatarBlueprint);

            // Create entity
            int playerID = t_entityHandler.CreateEntity(Blueprints::PlayerEntity);
            PlayerHandler::GetInstance()->Initialize(playerID);
            // Create the rigid body
            int materialID = t_entityHandler.GetComponentFromStorage<PhysicsMaterialComponent>(playerID)->p_materialID;
            XMFLOAT3 position = XMFLOAT3(0, 10, 0);
            XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
            RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<RigidBodyComponent>(playerID);
            bodyComp->p_bodyID =
                sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(position, orientation, XMFLOAT3(0.5, 0.5, 0.5), materialID);
            PlayerCreationEvent* playerCreationEvent = new PlayerCreationEvent();
            playerCreationEvent->eventType = Events::PlayerCreation;
            playerCreationEvent->playerEntityID = playerID;
            // EventHandler::GetInstance()->BroadcastEvent(playerCreationEvent);
        }


        void GameCore::LoadEngineLibrary()
        {
            // Load engine DLL
            m_engineLibrary = DynamicLoader::LoadSharedLibrary("EngineCore.dll");


            if(m_engineLibrary == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("1Failed to load engine - please check your installation.");
            }
        }


        void GameCore::InitializeClient()
        {
            START_ENGINE libInitializeEngine = (START_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StartEngine");

            if(libInitializeEngine == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            m_stopEngineFunction = (STOP_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StopEngine");

            if(m_stopEngineFunction == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            const DoremiEngine::Core::SharedContext& sharedContext = libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::ALL);

            /* This starts the physics handler. Should not be done here, but since this is the general
            code dump, it'll work for now TODOJB*/
            InputHandler::StartInputHandler(sharedContext);
            PlayerHandler::StartPlayerHandler(sharedContext);
            AudioHandler::StartAudioHandler(sharedContext);
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();


            ////////////////Example only////////////////
            // Create manager
            Manager* t_renderManager = new GraphicManager(sharedContext);
            Manager* t_physicsManager = new ExampleManager(sharedContext);
            // Manager* t_playerManager = new PlayerManager(sharedContext);
            Manager* t_clientNetworkManager = new ClientNetworkManager(sharedContext);
            Manager* t_movementManager = new MovementManager(sharedContext);
            Manager* t_audioManager = new AudioManager(sharedContext);
            Manager* t_cameraManager = new CameraManager(sharedContext);
            Manager* t_rigidTransSyndManager = new RigidTransformSyncManager(sharedContext);
            // Add manager to list of managers
            m_graphicalManagers.push_back(t_renderManager);
            m_managers.push_back(t_physicsManager);
            // m_managers.push_back(t_playerManager);
            m_managers.push_back(t_audioManager);
            m_managers.push_back(t_clientNetworkManager);
            m_managers.push_back(t_cameraManager);
            m_managers.push_back(t_rigidTransSyndManager);
            m_managers.push_back(t_movementManager);
            GenerateWorld(sharedContext);

            ////////////////End Example////////////////
        }

        void GameCore::InitializeServer()
        {
            START_ENGINE libInitializeEngine = (START_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StartEngine");

            if(libInitializeEngine == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            m_stopEngineFunction = (STOP_ENGINE)DynamicLoader::LoadProcess(m_engineLibrary, "StopEngine");

            if(m_stopEngineFunction == nullptr)
            {
                // TODORT proper logging
                throw std::runtime_error("Failed to load engine - please check your installation.");
            }

            const DoremiEngine::Core::SharedContext& sharedContext = libInitializeEngine(DoremiEngine::Core::EngineModuleEnum::NETWORK);
            InputHandler::StartInputHandler(sharedContext);

            /* This starts the physics handler. Should not be done here, but since this is the general
            code dump, it'll work for now TODOJB*/

            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            ////////////////Example only////////////////
            // Create manager

            Manager* t_physicsManager = new ExampleManager(sharedContext);
            Manager* t_serverNetworkManager = new ServerNetworkManager(sharedContext);


            // Add manager to list of managers
            m_managers.push_back(t_physicsManager);
            m_managers.push_back(t_serverNetworkManager);

            GenerateWorld(sharedContext);
            ////////////////End Example////////////////
        }

        void GameCore::UpdateGame(double p_deltaTime)
        {
            EventHandler::GetInstance()->DeliverEvents();
            PlayerHandler::GetInstance()->UpdatePosition();
            AudioHandler::GetInstance()->Update();
            // Have all managers update
            size_t length = m_managers.size();
            for(size_t i = 0; i < length; i++)
            {
                m_managers.at(i)->Update(p_deltaTime);
            }

            InputHandler::GetInstance()->Update();
        }

        void GameCore::DrawGame(double p_deltaTime)
        {
            size_t length = m_graphicalManagers.size();
            for(size_t i = 0; i < length; i++)
            {
                m_graphicalManagers.at(i)->Update(p_deltaTime);
            }
        }

        void GameCore::StartClientCore()
        {
            // TODOCM remove for better timer
            // GameLoop is not currently set
            uint64_t CurrentTime;
            uint64_t PreviousTime = GetTickCount64();
            uint64_t FrameTime = 0;
            uint64_t Accumulator = 0;
            uint64_t UpdateTimeStepLength = 17;
            uint64_t GameTime = 0;

            while(true)
            {
                CurrentTime = GetTickCount64();
                FrameTime = CurrentTime - PreviousTime;

                // We simulate maximum 250 milliseconds each frame
                // If we would let it be alone we would get mayor stops instead of lesser ones that will slowly catch up
                if(FrameTime > 250)
                {
                    FrameTime = 250;
                }
                // Update the previous position with frametime so we can catch up if we slow down
                PreviousTime += FrameTime;

                // Update Accumulator (how much we will work this frame)
                Accumulator += FrameTime;

                // Loop as many update-steps we will take this frame
                while(Accumulator >= UpdateTimeStepLength)
                {
                    // Update Game logic
                    UpdateGame(UpdateTimeStepLength / 1000.0f);

                    // Remove time from accumulator
                    Accumulator -= UpdateTimeStepLength;

                    // Add time to start
                    GameTime += UpdateTimeStepLength;
                }

                double alpha = Accumulator / UpdateTimeStepLength;

                // Interpolate the frames here


                // Draw stuff
                DrawGame(UpdateTimeStepLength / 1000.0f);
            }
        }

        void GameCore::StartServerCore()
        {
            // TODOCM remove for better timer
            // GameLoop is not currently set
            uint64_t CurrentTime;
            uint64_t PreviousTime = GetTickCount64();
            uint64_t FrameTime = 0;
            uint64_t Accumulator = 0;
            uint64_t UpdateTimeStepLength = 17;
            uint64_t GameTime = 0;

            while(true)
            {
                CurrentTime = GetTickCount64();
                FrameTime = CurrentTime - PreviousTime;

                // Update the previous position with frametime so we can catch up if we slow down&
                PreviousTime += FrameTime;

                // Update Accumulator (how much we will work this frame)
                Accumulator += FrameTime;

                // Loop as many update-steps we will take this frame
                while(Accumulator >= UpdateTimeStepLength)
                {
                    // Update Game logic
                    UpdateGame(UpdateTimeStepLength);

                    // Remove time from accumulator
                    Accumulator -= UpdateTimeStepLength;

                    // Add time to start
                    GameTime += UpdateTimeStepLength;
                }
            }
        }
    }
}