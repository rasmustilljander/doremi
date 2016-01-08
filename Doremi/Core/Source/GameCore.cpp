// Project specific
#include <GameCore.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <Manager/Manager.hpp>
#include <Manager/ExampleManager.hpp>
#include <Manager/AudioManager.hpp>
#include <Manager/Network/ClientNetworkManager.hpp>
#include <Manager/Network/ServerNetworkManager.hpp>
#include <Manager/GraphicManager.hpp>
#include <Manager/CameraManager.hpp>
#include <Manager/RigidTransformSyncManager.hpp>
#include <Manager/AIManager.hpp>
#include <Manager/CharacterControlSyncManager.hpp>
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
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EventHandler/Events/PlayerCreationEvent.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <InterpolationHandler.hpp>

#include <string>

// TODOCM remove for better timer?
#include <Windows.h>
#include <chrono>

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
            // TODORT Release the Managers... Waiting for refactor.
        }
        void CreateEnemyBlueprint(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityBlueprint blueprint;
            TransformComponent* transComp = new TransformComponent();
            blueprint[ComponentType::Transform] = transComp;
            // Render
            RenderComponent* renderComp = new RenderComponent();
            renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("AngryFace.dds");
            blueprint[ComponentType::Render] = renderComp;
            // PhysicsMaterialComp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0); // TODOJB remove p_
            blueprint[ComponentType::PhysicalMaterial] = t_physMatComp;
            // Rigid body comp
            // RigidBodyComponent* rigidBodyComp = new RigidBodyComponent();
            // blueprint[ComponentType::RigidBody] = rigidBodyComp;
            // Character control comp label
            blueprint[ComponentType::CharacterController];
            // Health comp
            HealthComponent* healthComponent = new HealthComponent();
            healthComponent->maxHealth = 100;
            healthComponent->currentHealth = healthComponent->maxHealth;
            blueprint[ComponentType::Health] = healthComponent;
            // Enemy ai agent comp
            blueprint[ComponentType::AIAgent];
            // Range comp
            RangeComponent* rangeComp = new RangeComponent();
            rangeComp->range = 4;
            blueprint[ComponentType::Range] = rangeComp;
            // PotentialField component
            PotentialFieldComponent* potentialComp = new PotentialFieldComponent();
            potentialComp->ChargedActor = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(DirectX::XMFLOAT3(0, 0, 0), -3, 5);
            blueprint[ComponentType::PotentialField] = potentialComp;
            // Movement comp
            MovementComponent* movementcomp = new MovementComponent();
            blueprint[ComponentType::Movement] = movementcomp;
            // Register blueprint
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::EnemyEntity, blueprint);
            // Create some enemies
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            for(size_t i = 0; i < 5; i++)
            {
                int entityID = t_entityHandler.CreateEntity(Blueprints::EnemyEntity);
                XMFLOAT3 position = DirectX::XMFLOAT3(0, 7 - (int)i, i * 5);
                XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
                int matID = EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(entityID)->p_materialID;
                // RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
                // rigidComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation,
                //                                                                                               XMFLOAT3(0.5, 0.5, 0.5), matID);
                sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(entityID, matID, position, XMFLOAT2(0.1, 0.5));
            }
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
            t_transformComp->scale.x = 4.0f;
            t_transformComp->scale.z = 4.0f;
            t_transformComp->scale.y = 0.1f;
            t_platform[ComponentType::Transform] = t_transformComp;
            // Physical material comp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.5, 0.5, 0.5);
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
            AudioHandler::GetInstance()->SetupContinuousRecording();
            AudioHandler::GetInstance()->StartContinuousRecording();
            // Add label for frequency Check
            t_platform[ComponentType::FrequencyAffected];
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlatformEntity, t_platform);

            for(size_t i = 0; i < 5; i++)
            {
                int entityID = t_entityHandler.CreateEntity(Blueprints::PlatformEntity);
                XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 10.0f - (float)i, i * 5.0f);
                XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
                int matID = EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(entityID)->p_materialID;
                RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
                rigidComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(entityID, position, orientation,
                                                                                                              XMFLOAT3(2.0f, 0.05f, 2.0f), matID);
            }
            for(size_t i = 0; i < 1; i++)
            {
                int entityID = t_entityHandler.CreateEntity(Blueprints::PlatformEntity);
                XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 10.0f - (float)i - 5, (i + 5) * 5.0f);
                XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
                int matID = EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(entityID)->p_materialID;
                RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
                rigidComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(entityID, position, orientation,
                                                                                                               XMFLOAT3(2.0f, 0.05f, 2.0f), matID);
            }
        }
        // Only for testing, should be removed! TODO
        void GameCore::GenerateWorld(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            GenerateDebugPlatforms(sharedContext);
            CreateBulletBlueprint(sharedContext);
            CreateEnemyBlueprint(sharedContext);
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
            // RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            // t_avatarBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;
            // Player component
            PlayerComponent* t_playerComp = new PlayerComponent();
            t_playerComp->isControllable = true;
            t_avatarBlueprint[ComponentType::Player] = t_playerComp;
            // Movement Component
            MovementComponent* t_movementComp = new MovementComponent();
            t_avatarBlueprint[ComponentType::Movement] = t_movementComp;
            // Potential field comp
            PotentialFieldComponent* potentialComp = new PotentialFieldComponent();
            potentialComp->ChargedActor = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(DirectX::XMFLOAT3(0, 0, 0), 6, 200);
            t_avatarBlueprint[ComponentType::PotentialField] = potentialComp;
            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlayerEntity, t_avatarBlueprint);

            // Create entity
            int playerID = t_entityHandler.CreateEntity(Blueprints::PlayerEntity);

            // Create the rigid body
            int materialID = t_entityHandler.GetComponentFromStorage<PhysicsMaterialComponent>(playerID)->p_materialID;
            XMFLOAT3 position = XMFLOAT3(0, 10, -5);
            XMFLOAT4 orientation = XMFLOAT4(0, 0, 0, 1);
            // RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<RigidBodyComponent>(playerID);
            // bodyComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(playerID, position, orientation,
            //                                                                                              XMFLOAT3(0.5, 0.5, 0.5), materialID);
            sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(playerID, materialID, position, XMFLOAT2(1, 1));
            EntityHandler::GetInstance().AddComponent(playerID, (int)ComponentType::CharacterController);
            PlayerCreationEvent* playerCreationEvent = new PlayerCreationEvent();
            playerCreationEvent->eventType = Events::PlayerCreation;
            playerCreationEvent->playerEntityID = playerID;

            // EventHandler::GetInstance()->BroadcastEvent(playerCreationEvent);


            // TODOCM remove, Jaws interpolation test
            EntityBlueprint t_jawsDebugBlueprint;
            RenderComponent* t_renderComp2 = new RenderComponent();
            t_renderComp2->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp2->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            t_jawsDebugBlueprint[ComponentType::Render] = t_renderComp2;

            t_transformComp = new TransformComponent();
            t_transformComp->position = XMFLOAT3(0, 0, 0);
            t_transformComp->rotation = XMFLOAT4(0, 0, 0, 1);
            t_jawsDebugBlueprint[ComponentType::Transform] = t_transformComp;

            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity, t_jawsDebugBlueprint);

            int NewEntityID = t_entityHandler.CreateEntity(Blueprints::JawsDebugEntity);

            t_entityHandler.AddComponent(NewEntityID, (int)ComponentType::NetworkObject);
            TransformComponentPrevious* tPrev = GetComponent<TransformComponentPrevious>(NewEntityID);
            TransformComponentNext* tNext = GetComponent<TransformComponentNext>(NewEntityID);

            tPrev->position = XMFLOAT3(0, 0, 0);
            tPrev->rotation = XMFLOAT4(0, 0, 0, 1);

            tNext->position = XMFLOAT3(100, 0, 0);
            XMStoreFloat4(&tNext->rotation, XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(1, 0, 0)), XM_PI));

            XMFLOAT4 test;
            XMStoreFloat4(&test, XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(1, 0, 0)), XM_PIDIV2));
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

        void GameCore::GenerateWorldServerJawsTest(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            EntityBlueprint t_jawsDebugBlueprint;

            TransformComponent* t_transformComp = new TransformComponent();
            t_transformComp->position = XMFLOAT3(0, 0, -10);
            t_transformComp->rotation = XMFLOAT4(0, 0, 0, 1);
            t_jawsDebugBlueprint[ComponentType::Transform] = t_transformComp;

            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_jawsDebugBlueprint[ComponentType::PhysicalMaterial] = t_physMatComp;

            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_jawsDebugBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;

            // Movement Component
            MovementComponent* t_movementComp = new MovementComponent();
            t_movementComp->maxSpeed = 5;
            t_jawsDebugBlueprint[ComponentType::Movement] = t_movementComp;

            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity, t_jawsDebugBlueprint);

            int NewEntityID = t_entityHandler.CreateEntity(Blueprints::JawsDebugEntity);

            t_entityHandler.AddComponent(NewEntityID, (int)ComponentType::NetworkObject);
            TransformComponentPrevious* tPrev = GetComponent<TransformComponentPrevious>(NewEntityID);
            TransformComponentNext* tNext = GetComponent<TransformComponentNext>(NewEntityID);

            int materialID = t_entityHandler.GetComponentFromStorage<PhysicsMaterialComponent>(NewEntityID)->p_materialID;

            RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<RigidBodyComponent>(NewEntityID);
            bodyComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(NewEntityID, t_transformComp->position,
                                                                                                          t_transformComp->rotation,
                                                                                                          DirectX::XMFLOAT3(0.5, 0.5, 0.5), materialID);

            EntityBlueprint t_jawsDebugBlueprint2;

            TransformComponent* t_transformComp2 = new TransformComponent();
            t_transformComp2->position = XMFLOAT3(0, 0, -5);
            t_transformComp2->rotation = XMFLOAT4(0, 0, 0, 1);
            t_jawsDebugBlueprint2[ComponentType::Transform] = t_transformComp2;


            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity2, t_jawsDebugBlueprint2);

            NewEntityID = t_entityHandler.CreateEntity(Blueprints::JawsDebugEntity2);

            t_entityHandler.AddComponent(NewEntityID, (int)ComponentType::NetworkObject);
        }

        void GameCore::GenerateWorldClientJawsTest(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            EntityBlueprint t_jawsDebugBlueprint;
            RenderComponent* t_renderComp2 = new RenderComponent();
            t_renderComp2->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp2->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            t_jawsDebugBlueprint[ComponentType::Render] = t_renderComp2;

            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_jawsDebugBlueprint[ComponentType::PhysicalMaterial] = t_physMatComp;

            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_jawsDebugBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;

            MovementComponent* t_movementComp = new MovementComponent();
            t_movementComp->maxSpeed = 5;
            t_jawsDebugBlueprint[ComponentType::Movement] = t_movementComp;

            TransformComponent* t_transformComp = new TransformComponent();
            t_transformComp->position = XMFLOAT3(0, 0, -10);
            t_transformComp->rotation = XMFLOAT4(0, 0, 0, 1);
            t_jawsDebugBlueprint[ComponentType::Transform] = t_transformComp;

            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity, t_jawsDebugBlueprint);

            int NewEntityID = t_entityHandler.CreateEntity(Blueprints::JawsDebugEntity);

            // t_entityHandler.AddComponent(NewEntityID, (int)ComponentType::NetworkObject);
            TransformComponentPrevious* tPrev = GetComponent<TransformComponentPrevious>(NewEntityID);
            TransformComponentNext* tNext = GetComponent<TransformComponentNext>(NewEntityID);


            int materialID = t_entityHandler.GetComponentFromStorage<PhysicsMaterialComponent>(NewEntityID)->p_materialID;

            RigidBodyComponent* bodyComp = t_entityHandler.GetComponentFromStorage<RigidBodyComponent>(NewEntityID);
            bodyComp->p_bodyID = sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(NewEntityID, t_transformComp->position,
                                                                                                          t_transformComp->rotation,
                                                                                                          XMFLOAT3(0.5, 0.5, 0.5), materialID);


            EntityBlueprint t_jawsDebugBlueprint2;

            TransformComponent* t_transformComp2 = new TransformComponent();
            t_transformComp2->position = XMFLOAT3(0, 0, -5);
            t_transformComp2->rotation = XMFLOAT4(0, 0, 0, 1);
            t_jawsDebugBlueprint2[ComponentType::Transform] = t_transformComp2;

            RenderComponent* t_renderComp3 = new RenderComponent();
            t_renderComp3->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp3->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            t_jawsDebugBlueprint2[ComponentType::Render] = t_renderComp3;

            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity2, t_jawsDebugBlueprint2);

            NewEntityID = t_entityHandler.CreateEntity(Blueprints::JawsDebugEntity2);

            t_entityHandler.AddComponent(NewEntityID, (int)ComponentType::NetworkObject);
        }


        const DoremiEngine::Core::SharedContext& GameCore::InitializeEngine(const size_t& p_engineModulesToStart)
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

            return libInitializeEngine(p_engineModulesToStart);
        }
    }
}