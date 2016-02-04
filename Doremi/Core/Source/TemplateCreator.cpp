#include <Doremi/Core/Include/TemplateCreator.hpp>

#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RenderComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/AudioComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/AudioActiveComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/ExampleComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/Example2Component.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PlayerComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/MovementComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PotentialFieldComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/AIGroupComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RangeComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/JumpComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/GravityComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/EntityTypeComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PressureParticleComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/PlatformPatrolComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TriggerComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/NetworkObjectComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/EntitySpawnerComponent.hpp>

#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <Doremi/Core/Include/LevelLoaderClient.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

#include <Doremi/Core/Include/AudioHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        TemplateCreator* TemplateCreator::m_singleton = nullptr;

        TemplateCreator* TemplateCreator::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new TemplateCreator();
            }
            return m_singleton;
        }

        TemplateCreator::TemplateCreator() {}

        TemplateCreator::~TemplateCreator() {}


        void CreateEmpty()
        {
            EntityBlueprint blueprint;
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::EmptyEntity, blueprint);
        }

        void CreateExperimentalParticlePressureBlueprintClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityBlueprint blueprint;
            // Transform Component
            TransformComponent* transComp = new TransformComponent();
            transComp->position = XMFLOAT3(0, 13, -2);
            transComp->scale = XMFLOAT3(0.1, 0.1, 0.1);
            blueprint[ComponentType::Transform] = transComp;
            // Render
            RenderComponent* renderComp = new RenderComponent();
            renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("AngryFace.dds");
            blueprint[ComponentType::Render] = renderComp;
            // Pressure particle comp
            ParticlePressureComponent* particleComp = new ParticlePressureComponent();
            blueprint[ComponentType::PressureParticleSystem] = particleComp;

            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::ExperimentalPressureParticleEntity, blueprint);
        }

        void CreateExperimentalParticlePressureBlueprintServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityBlueprint blueprint;
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::ExperimentalPressureParticleEntity, blueprint);
        }

        void CreatePotentialFieldDebugServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityBlueprint blueprint;
            TransformComponent* transComp = new TransformComponent();
            blueprint[ComponentType::Transform] = transComp;
            // PotentialField component
            PotentialFieldComponent* potentialComp = new PotentialFieldComponent();
            blueprint[ComponentType::PotentialField] = potentialComp;
            // PhysicsMaterialComp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.5, 0.5, 0.5);
            blueprint[ComponentType::PhysicalMaterial] = t_physMatComp;
            // Rigid body comp
            RigidBodyComponent* rigidBodyComp = new RigidBodyComponent();
            blueprint[ComponentType::RigidBody] = rigidBodyComp;
            // Network object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(1);
            blueprint[ComponentType::NetworkObject] = netObjComp;


            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::DebugPotentialFieldActor, blueprint);
        }

        void CreatePotentialFieldDebugClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityBlueprint blueprint;
            TransformComponent* transComp = new TransformComponent();
            blueprint[ComponentType::Transform] = transComp;
            RenderComponent* renderComp = new RenderComponent();
            renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("AngryFace.dds");
            blueprint[ComponentType::Render] = renderComp;

            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            blueprint[ComponentType::NetworkObject] = netObjComp;

            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::DebugPotentialFieldActor, blueprint);
        }

        void CreateEnemyBlueprintClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityBlueprint blueprint;
            TransformComponent* transComp = new TransformComponent();


            blueprint[ComponentType::Transform] = transComp;
            // Render
            RenderComponent* renderComp = new RenderComponent();
            renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("AngryFace.dds");
            blueprint[ComponentType::Render] = renderComp;
            // PhysicsMaterialComp
            // PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            // t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0); // TODOJB remove p_
            // blueprint[ComponentType::PhysicalMaterial] = t_physMatComp;
            // Rigid body comp
            // RigidBodyComponent* rigidBodyComp = new RigidBodyComponent();
            // blueprint[ComponentType::RigidBody] = rigidBodyComp;
            // Character control comp label
            // blueprint[ComponentType::CharacterController];
            // Health comp
            HealthComponent* healthComponent = new HealthComponent();
            healthComponent->maxHealth = 100;
            healthComponent->currentHealth = healthComponent->maxHealth;
            blueprint[ComponentType::Health] = healthComponent;
            // Enemy ai agent comp
            // blueprint[ComponentType::AIAgent];
            // Range comp
            /*RangeComponent* rangeComp = new RangeComponent();
            rangeComp->range = 4;
            blueprint[ComponentType::Range] = rangeComp;*/
            // PotentialField component
            /*PotentialFieldComponent* potentialComp = new PotentialFieldComponent();
            blueprint[ComponentType::PotentialField] = potentialComp;*/
            // AI group component
            /* AIGroupComponent* group = new AIGroupComponent();
             group->Group = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewPotentialGroup();
             blueprint[ComponentType::AIGroup] = group;*/
            // Movement comp
            /* MovementComponent* movementcomp = new MovementComponent();
             blueprint[ComponentType::Movement] = movementcomp;*/
            // Register blueprint

            // Network object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            blueprint[ComponentType::NetworkObject] = netObjComp;

            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::EnemyEntity, blueprint);
            TIME_FUNCTION_STOP
        }

        void CreateEnemyBlueprintServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityBlueprint blueprint;
            TransformComponent* transComp = new TransformComponent();
            blueprint[ComponentType::Transform] = transComp;

            // PhysicsMaterialComp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0); // TODOJB remove p_
            blueprint[ComponentType::PhysicalMaterial] = t_physMatComp;

            // Character control comp label
            blueprint[ComponentType::CharacterController];

            // Health comp
            HealthComponent* healthComponent = new HealthComponent();
            healthComponent->maxHealth = 100;
            healthComponent->currentHealth = 100;
            blueprint[ComponentType::Health] = healthComponent;
            // Enemy ai agent comp
            blueprint[ComponentType::AIAgent];

            // Range comp
            RangeComponent* rangeComp = new RangeComponent();
            rangeComp->range = 30;
            blueprint[ComponentType::Range] = rangeComp;

            // PotentialField component
            PotentialFieldComponent* potentialComp = new PotentialFieldComponent();
            blueprint[ComponentType::PotentialField] = potentialComp;

            // AI group component
            AIGroupComponent* group = new AIGroupComponent();
            group->Group = sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewPotentialGroup();
            blueprint[ComponentType::AIGroup] = group;

            // Movement comp
            MovementComponent* movementcomp = new MovementComponent();
            blueprint[ComponentType::Movement] = movementcomp;

            // Gravity comp
            GravityComponent* gravComp = new GravityComponent();
            blueprint[ComponentType::Gravity] = gravComp;
            // Network object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(2.0f);
            blueprint[ComponentType::NetworkObject] = netObjComp;

            // Register blueprint
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::EnemyEntity, blueprint);
            TIME_FUNCTION_STOP
        }

        void CreateBulletBlueprintServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            /// Create Blueprint
            EntityBlueprint blueprint;

            // Add components to blueprint

            // Transform
            TransformComponent* transComp = new TransformComponent();
            transComp->scale = DirectX::XMFLOAT3(0.5, 0.5, 0.5);
            blueprint[ComponentType::Transform] = transComp;

            // PhysicsMaterialComp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.5, 0.5, 0.5);
            blueprint[ComponentType::PhysicalMaterial] = t_physMatComp;

            // Rigid body comp
            RigidBodyComponent* rigidBodyComp = new RigidBodyComponent();
            blueprint[ComponentType::RigidBody] = rigidBodyComp;

            // Entity type comp
            EntityTypeComponent* typeComp = new EntityTypeComponent();
            blueprint[ComponentType::EntityType] = typeComp;

            // Network object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(1.0f);
            blueprint[ComponentType::NetworkObject] = netObjComp;

            /// Register blueprint
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::BulletEntity, blueprint);
            TIME_FUNCTION_STOP
        }

        void CreateBulletBlueprintClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            /// Create Blueprint
            EntityBlueprint blueprint;

            // Add components to blueprint

            // Transform
            TransformComponent* transComp = new TransformComponent();
            transComp->scale = DirectX::XMFLOAT3(0.5, 0.5, 0.5);
            blueprint[ComponentType::Transform] = transComp;

            // Render
            RenderComponent* renderComp = new RenderComponent();
            renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            blueprint[ComponentType::Render] = renderComp;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            blueprint[ComponentType::NetworkObject] = netObjComp;

            /// Register blueprint
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::BulletEntity, blueprint);
            TIME_FUNCTION_STOP
        }

        void CreateDebugPlatformsServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create blueprint
            EntityBlueprint t_platform;

            // Add components

            // Transform
            TransformComponent* t_transformComp = new TransformComponent();
            t_transformComp->scale.x = 10.0f;
            t_transformComp->scale.z = 10.0f;
            t_transformComp->scale.y = 0.1f;
            t_platform[ComponentType::Transform] = t_transformComp;

            // Physical material comp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_platform[ComponentType::PhysicalMaterial] = t_physMatComp;

            // Rigid body comp
            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_platform[ComponentType::RigidBody] = t_rigidBodyComp;

            // Add label for frequency Check
            t_platform[ComponentType::FrequencyAffected];

            // Add component for frequency velocity add
            PlatformPatrolComponent* t_platformPatrolComponent = new PlatformPatrolComponent();
            t_platform[ComponentType::PlatFormPatrolComponent] = t_platformPatrolComponent;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(1);
            t_platform[ComponentType::NetworkObject] = netObjComp;

            // Register bluepirnt
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlatformEntity, t_platform);
            TIME_FUNCTION_STOP
        }

        void CreateDebugPlatformsClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create blueprint
            EntityBlueprint t_platform;

            // Add components

            // Render
            RenderComponent* t_renderComp = new RenderComponent();
            t_renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("PlayButton.dds");
            t_platform[ComponentType::Render] = t_renderComp;

            // Transform
            TransformComponent* t_transformComp = new TransformComponent();
            t_transformComp->scale.x = 10.0f;
            t_transformComp->scale.z = 10.0f;
            t_transformComp->scale.y = 0.1f;
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
            t_audioComponent->soundID =
                sharedContext.GetAudioModule().LoadSound("Sounds/Test sounds/1 amp som har låg frekv sen hög, Human made!372hz till 643hz.wav", 0.5f, 5000.0f);
            t_platform[ComponentType::Audio] = t_audioComponent;

            // ActiveAudiocomp
            AudioActiveComponent* t_audioActiveComponent = new AudioActiveComponent();
            AudioHandler::GetInstance()->Initialize();
            sharedContext.GetAudioModule().PlayASound(t_audioComponent->soundID, true, t_audioActiveComponent->channelID);
            sharedContext.GetAudioModule().SetVolumeOnChannel(t_audioActiveComponent->channelID, 0.0f);
            t_platform[ComponentType::AudioActive] = t_audioActiveComponent;
            AudioHandler::GetInstance()->SetupContinuousRecording();
            AudioHandler::GetInstance()->StartContinuousRecording();
            AudioHandler::GetInstance()->SetupRepeatableRecording();


            // Add label for frequency Check
            t_platform[ComponentType::FrequencyAffected];

            // Add component for frequency velocity add
            PlatformPatrolComponent* t_platformPatrolComponent = new PlatformPatrolComponent();
            t_platform[ComponentType::PlatFormPatrolComponent] = t_platformPatrolComponent;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            t_platform[ComponentType::NetworkObject] = netObjComp;

            // Register bluepirnt
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlatformEntity, t_platform);
            TIME_FUNCTION_STOP
        }

        void CreatePlayerServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create blueprint
            EntityBlueprint t_avatarBlueprint;

            // Add components

            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_avatarBlueprint[ComponentType::Transform] = t_transformComp;

            // Physical material comp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_avatarBlueprint[ComponentType::PhysicalMaterial] = t_physMatComp;

            // Rigid body comp
            // RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            // t_avatarBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;

            // Character Controller
            t_avatarBlueprint[ComponentType::CharacterController];

            // Player component
            PlayerComponent* t_playerComp = new PlayerComponent();
            t_playerComp->isControllable = true;
            t_avatarBlueprint[ComponentType::Player] = t_playerComp;

            // Movement Component
            MovementComponent* t_movementComp = new MovementComponent();
            t_avatarBlueprint[ComponentType::Movement] = t_movementComp;

            // Jump component
            JumpComponent* jumpComp = new JumpComponent();
            jumpComp->intensity = 0.3f;

            // jumpComp->jumpTime = 0.5;
            t_avatarBlueprint[ComponentType::Jump] = jumpComp;

            // Potential field component
            PotentialFieldComponent* potentialComp = new PotentialFieldComponent();
            t_avatarBlueprint[ComponentType::PotentialField] = potentialComp;

            // Gravity component
            GravityComponent* gravComp = new GravityComponent();
            t_avatarBlueprint[ComponentType::Gravity] = gravComp;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(3);
            t_avatarBlueprint[ComponentType::NetworkObject] = netObjComp;

            // Range Component
            RangeComponent* t_rangeComponent = new RangeComponent();
            t_rangeComponent->range = 20.0f;
            t_avatarBlueprint[ComponentType::Range] = t_rangeComponent;

            // Pressure particle comp
            ParticlePressureComponent* particleComp = new ParticlePressureComponent();
            t_avatarBlueprint[ComponentType::PressureParticleSystem] = particleComp;

            // HP component
            HealthComponent* hpComp = new HealthComponent();
            hpComp->currentHealth = 50;
            hpComp->maxHealth = 50;
            t_avatarBlueprint[ComponentType::Health] = hpComp;
            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlayerEntity, t_avatarBlueprint);
            TIME_FUNCTION_STOP
        }

        void CreatePlayerClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            EntityBlueprint t_avatarBlueprint;

            LevelLoaderClient loader = LevelLoaderClient(sharedContext);

            CharacterDataNames playerCharData = loader.LoadCharacter("Models/testCharCoolGuy.drm");

            /// Fill with components
            // Render
            RenderComponent* t_renderComp = new RenderComponent();
            t_renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo(playerCharData.meshName);
            t_renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo(playerCharData.materialName);
            t_avatarBlueprint[ComponentType::Render] = t_renderComp;

            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_avatarBlueprint[ComponentType::Transform] = t_transformComp;
            t_transformComp->scale = XMFLOAT3(0.25f, 0.25f, 0.25f);

            // Physical material comp
            PhysicsMaterialComponent* t_physMatComp = new PhysicsMaterialComponent();
            t_physMatComp->p_materialID = sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            t_avatarBlueprint[ComponentType::PhysicalMaterial] = t_physMatComp;

            // Rigid body comp
            // RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            // t_avatarBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;

            // Controller component
            t_avatarBlueprint[ComponentType::CharacterController];

            // Player component
            PlayerComponent* t_playerComp = new PlayerComponent();
            t_playerComp->isControllable = true;
            t_avatarBlueprint[ComponentType::Player] = t_playerComp;

            // Movement Component
            MovementComponent* t_movementComp = new MovementComponent();
            t_avatarBlueprint[ComponentType::Movement] = t_movementComp;

            // Jump component
            JumpComponent* jumpComp = new JumpComponent();
            jumpComp->intensity = 0.3f;
            // jumpComp->jumpTime = 0.5;
            t_avatarBlueprint[ComponentType::Jump] = jumpComp;

            // Gravity component
            GravityComponent* t_gravComp = new GravityComponent();
            t_avatarBlueprint[ComponentType::Gravity] = t_gravComp;

            // Range Component
            RangeComponent* t_rangeComponent = new RangeComponent();
            t_rangeComponent->range = 20.0f;
            t_avatarBlueprint[ComponentType::Range] = t_rangeComponent;

            // Pressure particle comp
            ParticlePressureComponent* particleComp = new ParticlePressureComponent();
            particleComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            particleComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("test.dds");
            t_avatarBlueprint[ComponentType::PressureParticleSystem] = particleComp;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            t_avatarBlueprint[ComponentType::NetworkObject] = netObjComp;

            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlayerEntity, t_avatarBlueprint);
            TIME_FUNCTION_STOP
        }

        void CreateNetworkPlayerClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START

            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            EntityBlueprint t_avatarBlueprint;

            LevelLoaderClient loader = LevelLoaderClient(sharedContext);

            CharacterDataNames playerCharData = loader.LoadCharacter("Models/testCharCoolGuy.drm");

            /// Fill with components
            // Render
            RenderComponent* t_renderComp = new RenderComponent();
            t_renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo(playerCharData.meshName);
            t_renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo(playerCharData.materialName);
            t_avatarBlueprint[ComponentType::Render] = t_renderComp;

            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_avatarBlueprint[ComponentType::Transform] = t_transformComp;
            t_transformComp->scale = XMFLOAT3(0.25f, 0.25f, 0.25f);

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            t_avatarBlueprint[ComponentType::NetworkObject] = netObjComp;

            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::NetworkPlayerEntity, t_avatarBlueprint);

            TIME_FUNCTION_STOP
        }

        void CreateJawsDebugObjectServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create blueprint
            EntityBlueprint t_avatarBlueprint;

            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_avatarBlueprint[ComponentType::Transform] = t_transformComp;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(1);
            t_avatarBlueprint[ComponentType::NetworkObject] = netObjComp;

            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity, t_avatarBlueprint);
            TIME_FUNCTION_STOP
        }

        void CreateJawsDebugObjectClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            EntityBlueprint t_avatarBlueprint;

            /// Fill with components
            // Render
            RenderComponent* t_renderComp = new RenderComponent();
            t_renderComp->mesh = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("hej");
            t_renderComp->material = sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("Test.dds");
            t_avatarBlueprint[ComponentType::Render] = t_renderComp;

            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_avatarBlueprint[ComponentType::Transform] = t_transformComp;

            // Net object
            NetworkObjectComponent* netObjComp = new NetworkObjectComponent(0);
            t_avatarBlueprint[ComponentType::NetworkObject] = netObjComp;


            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::JawsDebugEntity, t_avatarBlueprint);
            TIME_FUNCTION_STOP
        }
        void CreateTriggerObjectClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            EntityBlueprint t_triggerBlueprint;
            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_triggerBlueprint[ComponentType::Transform] = t_transformComp;
            // RIgid body comp
            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_triggerBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;
            // Trigger comp
            TriggerComponent* t_triggerComp = new TriggerComponent();
            t_triggerBlueprint[ComponentType::Trigger] = t_triggerComp;
            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::TriggerEntity, t_triggerBlueprint);
            TIME_FUNCTION_STOP
        }
        void CreateTriggerObjectServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            TIME_FUNCTION_START
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            EntityBlueprint t_triggerBlueprint;
            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_triggerBlueprint[ComponentType::Transform] = t_transformComp;
            // RIgid body comp
            RigidBodyComponent* t_rigidBodyComp = new RigidBodyComponent();
            t_triggerBlueprint[ComponentType::RigidBody] = t_rigidBodyComp;
            // Trigger comp
            TriggerComponent* t_triggerComp = new TriggerComponent();
            t_triggerBlueprint[ComponentType::Trigger] = t_triggerComp;
            // Register blueprint
            t_entityHandler.RegisterEntityBlueprint(Blueprints::TriggerEntity, t_triggerBlueprint);
            TIME_FUNCTION_STOP
        }

        void CreateSpawnerEntityClient()
        {
            // Empty by design: exists only on server
            EntityBlueprint t_blueprint;
            TransformComponent* t_transformComp = new TransformComponent();
            t_blueprint[ComponentType::Transform] = t_transformComp;
            EntityHandler::GetInstance().RegisterEntityBlueprint(Blueprints::EnemySpawnerEntity, t_blueprint);
        }
        void CreateSpawnerEntityServer()
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            EntityBlueprint t_blueprint;

            // Transform comp
            TransformComponent* t_transformComp = new TransformComponent();
            t_blueprint[ComponentType::Transform] = t_transformComp;

            // Spawn component
            EntitySpawnComponent* t_entitySpawnComp = new EntitySpawnComponent();
            t_entitySpawnComp->entityBlueprint = Blueprints::EnemyEntity;
            t_entitySpawnComp->spawnRadius = 2;
            t_entitySpawnComp->timeBetweenSpawns = 2;
            t_entitySpawnComp->type = SpawnerType::TimedSpawner;
            t_entitySpawnComp->maxNumSpawnedEntites = 4;
            t_blueprint[ComponentType::EntitySpawner] = t_entitySpawnComp;

            t_entityHandler.RegisterEntityBlueprint(Blueprints::EnemySpawnerEntity, t_blueprint);
        }
        void TemplateCreator::CreateTemplatesForClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            CreateDebugPlatformsClient(sharedContext);
            CreateBulletBlueprintClient(sharedContext);
            CreatePlayerClient(sharedContext);
            CreateNetworkPlayerClient(sharedContext);
            CreateEnemyBlueprintClient(sharedContext);
            CreateJawsDebugObjectClient(sharedContext);
            CreateEmpty();
            CreateExperimentalParticlePressureBlueprintClient(sharedContext);
            CreatePotentialFieldDebugClient(sharedContext);
            CreateTriggerObjectClient(sharedContext);
            CreateSpawnerEntityClient();
        }

        void TemplateCreator::CreateTemplatesForServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            CreateDebugPlatformsServer(sharedContext);
            CreateBulletBlueprintServer(sharedContext);
            CreatePlayerServer(sharedContext);
            CreateEnemyBlueprintServer(sharedContext);
            CreateJawsDebugObjectServer(sharedContext);
            CreateEmpty();
            CreateExperimentalParticlePressureBlueprintServer(sharedContext);
            CreatePotentialFieldDebugServer(sharedContext);
            CreateTriggerObjectServer(sharedContext);
            CreateSpawnerEntityServer();
        }
    }
}