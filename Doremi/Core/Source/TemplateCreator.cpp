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


#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>

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

        void CreateBulletBlueprintServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            /// Create Blueprint
            EntityBlueprint blueprint;

            // Add components to blueprint

            // Transform
            TransformComponent* transComp = new TransformComponent();
            blueprint[ComponentType::Transform] = transComp;

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

        void CreateBulletBlueprintClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            /// Create Blueprint
            EntityBlueprint blueprint;

            // Add components to blueprint

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

        void CreateDebugPlatformsServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create blueprint
            EntityBlueprint t_platform;

            // Add components

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

            // Add label for frequency Check
            t_platform[ComponentType::FrequencyAffected];

            // Register bluepirnt
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlatformEntity, t_platform);
        }

        void CreateDebugPlatformsClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Create blueprint
            EntityBlueprint t_platform;

            // Add components

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
            t_audioComponent->soundID =
                sharedContext.GetAudioModule().LoadSound("Sounds/Test sounds/1 amp som har låg frekv sen hög, Human made!372hz till 643hz.wav", 0.5f, 5000.0f);
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

            // Register bluepirnt
            t_entityHandler.RegisterEntityBlueprint(Blueprints::PlatformEntity, t_platform);
        }

        void CreatePlayerServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
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
        }

        void CreatePlayerClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
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
        }

        void TemplateCreator::CreateTemplatesForClient(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            CreateDebugPlatformsClient(sharedContext);
            CreateBulletBlueprintClient(sharedContext);
            CreatePlayerClient(sharedContext);
        }

        void TemplateCreator::CreateTemplatesForServer(const DoremiEngine::Core::SharedContext& sharedContext)
        {
            CreateDebugPlatformsServer(sharedContext);
            CreateBulletBlueprintServer(sharedContext);
            CreatePlayerServer(sharedContext);
        }
    }
}