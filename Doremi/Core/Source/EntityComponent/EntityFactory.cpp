// Project specific
#include <EntityComponent/EntityFactory.hpp>
#include <EntityComponent/EntityManager.hpp>
#include <EntityComponent/ComponentTable.hpp>
#include <EntityComponent/StorageShelf.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <EntityComponent/Components/AudioActiveComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/AIGroupComponent.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/PressureParticleComponent.hpp>
#include <EntityComponent/Components/LightComponent.hpp>
#include <EntityComponent/Components/PlatformPatrolComponent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/DamageInflictorsComponent.hpp>
#include <EntityComponent/Components/NetworkObjectComponent.hpp>
#include <EntityComponent/Components/EntitySpawnerComponent.hpp>
#include <EntityComponent/Components/CharacterControlComponen.hpp>

// Events
#include <EventHandler/Events/SpecialEntityCreatedEvent.hpp>
// Handlers
#include <EventHandler/EventHandler.hpp>

/// Engine
// Core
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <EntityComponent/Components/SkeletalAnimationComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        EntityFactory* EntityFactory::mSingleton = nullptr;

        EntityFactory* EntityFactory::GetInstance() { return mSingleton; }

        void EntityFactory::StartupEntityFactory(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(mSingleton == nullptr)
            {
                mSingleton = new EntityFactory(p_sharedContext);
            }
        }


        // TODORT Make sure this is called.
        void EntityFactory::ReleaseInstance()
        {
            delete mSingleton;
            mSingleton = nullptr;
        }

        void EntityFactory::ScrapEntity(int p_entityID)
        {
            DoremiEngine::Physics::PhysicsModule& physicsModule = m_sharedContext.GetPhysicsModule();
            EntityManager* tEntityManager = tEntityManager->GetInstance();
            ComponentTable* tComponentTable = tComponentTable->GetInstance();
            if(tComponentTable->HasComponent(p_entityID, (int)ComponentType::RigidBody))
            {
                physicsModule.GetRigidBodyManager().RemoveBody(p_entityID);
            }
            if(tComponentTable->HasComponent(p_entityID, (int)ComponentType::CharacterController))
            {
                physicsModule.GetCharacterControlManager().RemoveCharacterController(p_entityID);
            }
            if(tComponentTable->HasComponent(p_entityID, (int)ComponentType::PressureParticleSystem))
            {
                // physicsModule.GetFluidManager() //Remove particle system here TODOJB actually do this
            }
        }

        EntityFactory::EntityFactory(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        EntityFactory::~EntityFactory() {}

        void EntityFactory::CreateComponents(EntityID p_entityID, Blueprints p_blueprintID)
        {
            EntityBlueprint tComponentMap = mEntityBlueprints[p_blueprintID];
            EntityManager* tEntityManager = tEntityManager->GetInstance();
            ComponentTable* tComponentTable = tComponentTable->GetInstance();

            for(EntityBlueprint::iterator iter = tComponentMap.begin(); iter != tComponentMap.end(); ++iter)
            {


                // Check which component we copy
                if(iter->first == ComponentType::Example)
                {
                    memcpy(GetComponent<ExampleComponent>(p_entityID), iter->second, sizeof(ExampleComponent));
                }
                else if(iter->first == ComponentType::Example2)
                {
                    memcpy(GetComponent<Example2Component>(p_entityID), iter->second, sizeof(Example2Component));
                }
                else if(iter->first == ComponentType::Audio)
                {
                    memcpy(GetComponent<AudioComponent>(p_entityID), iter->second, sizeof(AudioComponent));
                }
                else if(iter->first == ComponentType::AudioActive)
                {
                    memcpy(GetComponent<AudioActiveComponent>(p_entityID), iter->second, sizeof(AudioActiveComponent));
                }
                else if(iter->first == ComponentType::Render)
                {
                    memcpy(GetComponent<RenderComponent>(p_entityID), iter->second, sizeof(RenderComponent));
                }
                else if(iter->first == ComponentType::Transform)
                {
                    // We we already got one from the position CreateEntity overload, we don't need to copy the blueprint's
                    if(!tComponentTable->HasComponent(p_entityID, (int)ComponentType::Transform))
                    {
                        // memcpy(GetComponent<TransformComponent>(p_entityID), iter->second, sizeof(TransformComponent));
                    }
                }
                else if(iter->first == ComponentType::RigidBody)
                {
                    // Get the transform component so we can acces our position and orientation (Bold but works)
                    TransformComponent* transComp = GetComponent<TransformComponent>(p_entityID);
                    // Get us our rigid body manager
                    DoremiEngine::Physics::RigidBodyManager& rigidBodyManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();
                    // Copy the data
                    memcpy(GetComponent<RigidBodyComponent>(p_entityID), iter->second, sizeof(RigidBodyComponent));
                    // Get our body comp
                    RigidBodyComponent* bodyComp = reinterpret_cast<RigidBodyComponent*>(iter->second);

                    /// Create the body
                    // Get the material. This is haxxy. It probably works most of the time
                    PhysicsMaterialComponent* matComp =
                        reinterpret_cast<PhysicsMaterialComponent*>(mEntityBlueprints[p_blueprintID][ComponentType::PhysicalMaterial]);
                    // Create the bodies
                    switch(bodyComp->geometry)
                    {
                        case RigidBodyGeometry::dynamicBox:
                            rigidBodyManager.AddBoxBodyDynamic(p_entityID, transComp->position, transComp->rotation, bodyComp->boxDims, matComp->p_materialID);
                            break;
                        case RigidBodyGeometry::dynamicSphere:
                            rigidBodyManager.AddSphereBodyDynamic(p_entityID, transComp->position, bodyComp->radius);
                            break;
                        case RigidBodyGeometry::dynamicCapsule:
                            rigidBodyManager.AddCapsuleBodyDynamic(p_entityID, transComp->position, transComp->rotation, bodyComp->height, bodyComp->radius);
                            break;
                        case RigidBodyGeometry::staticBox:
                            rigidBodyManager.AddBoxBodyStatic(p_entityID, transComp->position, transComp->rotation, bodyComp->boxDims, matComp->p_materialID);
                            break;
                        default:
                            break;
                    }
                    // Apply flags
                    if(((int)bodyComp->flags & (int)RigidBodyFlags::kinematic) == (int)RigidBodyFlags::kinematic)
                    {
                        rigidBodyManager.SetKinematicActor(p_entityID, true);
                    }
                    if(((int)bodyComp->flags & (int)RigidBodyFlags::trigger) == (int)RigidBodyFlags::trigger)
                    {
                        rigidBodyManager.SetTrigger(p_entityID, true);
                    }
                    if(((int)bodyComp->flags & (int)RigidBodyFlags::drain) == (int)RigidBodyFlags::drain)
                    {
                        rigidBodyManager.SetDrain(p_entityID, true);
                    }
                    if(((int)bodyComp->flags & (int)RigidBodyFlags::ignoredDEBUG) == (int)RigidBodyFlags::ignoredDEBUG)
                    {
                        rigidBodyManager.SetIgnoredDEBUG(p_entityID);
                    }
                }

                else if(iter->first == ComponentType::CharacterController)
                {
                    // Get the transform component
                    TransformComponent* transComp = GetComponent<TransformComponent>(p_entityID);
                    // Get the character control manager
                    DoremiEngine::Physics::CharacterControlManager& characterControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();
                    // Copy the data
                    memcpy(GetComponent<CharacterControlComponent>(p_entityID), iter->second, sizeof(CharacterControlComponent));
                    // Get our body comp
                    CharacterControlComponent* controlComp = reinterpret_cast<CharacterControlComponent*>(iter->second);

                    /// Create the body
                    // Get the material. This is haxxy. It probably works most of the time
                    PhysicsMaterialComponent* matComp =
                        reinterpret_cast<PhysicsMaterialComponent*>(mEntityBlueprints[p_blueprintID][ComponentType::PhysicalMaterial]);
                    // Create the controller
                    characterControlManager.AddController(p_entityID, matComp->p_materialID, transComp->position, controlComp->dims);

                    // Apply flags
                    if(((int)controlComp->flags & (int)CharacterControlFlags::drain) == (int)CharacterControlFlags::drain)
                    {
                        characterControlManager.SetDrain(p_entityID, true);
                    }
                }
                else if(iter->first == ComponentType::PhysicalMaterial)
                {
                    memcpy(GetComponent<PhysicsMaterialComponent>(p_entityID), iter->second, sizeof(PhysicsMaterialComponent));
                }
                else if(iter->first == ComponentType::Movement)
                {
                    memcpy(GetComponent<MovementComponent>(p_entityID), iter->second, sizeof(MovementComponent));
                }
                else if(iter->first == ComponentType::Health)
                {
                    memcpy(GetComponent<HealthComponent>(p_entityID), iter->second, sizeof(HealthComponent));
                }
                else if(iter->first == ComponentType::Range)
                {
                    memcpy(GetComponent<RangeComponent>(p_entityID), iter->second, sizeof(RangeComponent));
                }
                else if(iter->first == ComponentType::PotentialField)
                {
                    // Get the transform component
                    TransformComponent* transComp = GetComponent<TransformComponent>(p_entityID);
                    // Get potential field submodule
                    DoremiEngine::AI::PotentialFieldSubModule& fieldModule = m_sharedContext.GetAIModule().GetPotentialFieldSubModule();
                    memcpy(GetComponent<PotentialFieldComponent>(p_entityID), iter->second, sizeof(PotentialFieldComponent));
                    PotentialFieldComponent* thisComp = GetComponent<PotentialFieldComponent>(p_entityID);
                    thisComp->ChargedActor =
                        fieldModule.CreateNewActor(transComp->position, thisComp->charge, thisComp->range, thisComp->isStatic, thisComp->type);
                    if(false) // thisComp->type == DoremiEngine::AI::AIActorType::Enemy)
                    {
                        SpecialEntityCreatedEvent* AIGroupActorCreated = new Core::SpecialEntityCreatedEvent(p_entityID, Core::EventType::AiGroupActorCreation);
                        EventHandler::GetInstance()->BroadcastEvent(AIGroupActorCreated);
                    }
                }
                else if(iter->first == ComponentType::AIGroup)
                {
                    memcpy(GetComponent<AIGroupComponent>(p_entityID), iter->second, sizeof(AIGroupComponent));
                }
                else if(iter->first == ComponentType::Jump)
                {
                    memcpy(GetComponent<JumpComponent>(p_entityID), iter->second, sizeof(JumpComponent));
                }
                else if(iter->first == ComponentType::Gravity)
                {
                    memcpy(GetComponent<GravityComponent>(p_entityID), iter->second, sizeof(GravityComponent));
                }
                else if(iter->first == ComponentType::EntityType)
                {
                    memcpy(GetComponent<EntityTypeComponent>(p_entityID), iter->second, sizeof(EntityTypeComponent));
                }
                else if(iter->first == ComponentType::PressureParticleSystem)
                {
                    memcpy(GetComponent<ParticlePressureComponent>(p_entityID), iter->second, sizeof(ParticlePressureComponent));
                    ParticlePressureComponent* thisComp = GetComponent<ParticlePressureComponent>(p_entityID);
                    m_sharedContext.GetPhysicsModule().GetFluidManager().CreateParticleEmitter(p_entityID, thisComp->data);
                }
                else if(iter->first == ComponentType::Light)
                {
                    memcpy(GetComponent<LightComponent>(p_entityID), iter->second, sizeof(LightComponent));
                }
                else if(iter->first == ComponentType::PlatFormPatrolComponent)
                {
                    memcpy(GetComponent<PlatformPatrolComponent>(p_entityID), iter->second, sizeof(PlatformPatrolComponent));
                }
                else if(iter->first == ComponentType::Trigger)
                {
                    memcpy(GetComponent<TriggerComponent>(p_entityID), iter->second, sizeof(TriggerComponent));
                }
                else if(iter->first == ComponentType::DamageInflictors)
                {
                    memcpy(GetComponent<DamageInflictorsComponent>(p_entityID), iter->second, sizeof(DamageInflictorsComponent));
                }
                else if(iter->first == ComponentType::NetworkObject)
                {
                    memcpy(GetComponent<NetworkObjectComponent>(p_entityID), iter->second, sizeof(NetworkObjectComponent));
                }
                else if(iter->first == ComponentType::EntitySpawner)
                {
                    memcpy(GetComponent<EntitySpawnComponent>(p_entityID), iter->second, sizeof(EntitySpawnComponent));
                }
                else if (iter->first == ComponentType::SkeletalAnimation)
                {
                    memcpy(GetComponent<SkeletalAnimationComponent>(p_entityID), iter->second, sizeof(SkeletalAnimationComponent));
                }

                // Add bitmask. This is now done last due to transform being a dick
                tComponentTable->AddComponent(p_entityID, (int)iter->first);
                
            }
        }

        void EntityFactory::Initialize() {}

        void EntityFactory::RegisterEntityTemplate(Blueprints p_blueprintID, EntityBlueprint pComponents)
        {
            mEntityBlueprints[p_blueprintID] = pComponents;
        }

        EntityID EntityFactory::CreateEntity(Blueprints p_blueprintID)
        {
            EntityManager* tEntityManager = tEntityManager->GetInstance();

            // create a new ID
            EntityID tNewEntityID = tEntityManager->AddEntity();
            // Special case for transform component
            ComponentTable::GetInstance()->AddComponent(tNewEntityID, (int)ComponentType::Transform);
            TransformComponent* transComp = GetComponent<TransformComponent>(tNewEntityID);
            transComp->position = XMFLOAT3(0, 0, 0);
            transComp->rotation = XMFLOAT4(0, 0, 0, 1);
            transComp->scale = XMFLOAT3(1, 1, 1);

            CreateComponents(tNewEntityID, p_blueprintID);
            return tNewEntityID;
        }

        EntityID EntityFactory::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position)
        {
            EntityManager* tEntityManager = tEntityManager->GetInstance();
            EntityID tNewEntityID = tEntityManager->AddEntity();

            ComponentTable::GetInstance()->AddComponent(tNewEntityID, (int)ComponentType::Transform);
            TransformComponent* transComp = GetComponent<TransformComponent>(tNewEntityID);
            transComp->position = p_position;
            transComp->rotation = XMFLOAT4(0, 0, 0, 1);
            transComp->scale = XMFLOAT3(1, 1, 1);

            memcpy(GetComponent<TransformComponentNext>(tNewEntityID), transComp, sizeof(TransformComponent));
            memcpy(GetComponent<TransformComponentPrevious>(tNewEntityID), transComp, sizeof(TransformComponent));
            *GetComponent<TransformComponentSnapshotNext>(tNewEntityID) = TransformComponentSnapshotNext(*GetComponent<TransformComponentNext>(tNewEntityID));
            *GetComponent<TransformComponentSnapshotPrevious>(tNewEntityID) =
                TransformComponentSnapshotPrevious(*GetComponent<TransformComponentNext>(tNewEntityID));

            CreateComponents(tNewEntityID, p_blueprintID);

            return tNewEntityID;
        }

        EntityID EntityFactory::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation)
        {
            EntityManager* tEntityManager = tEntityManager->GetInstance();
            EntityID tNewEntityID = tEntityManager->AddEntity();

            TransformComponent* transComp = GetComponent<TransformComponent>(tNewEntityID);
            transComp->position = p_position;
            transComp->rotation = p_orientation;
            transComp->scale = XMFLOAT3(1, 1, 1);

            memcpy(GetComponent<TransformComponentNext>(tNewEntityID), transComp, sizeof(TransformComponent));
            memcpy(GetComponent<TransformComponentPrevious>(tNewEntityID), transComp, sizeof(TransformComponent));
            *GetComponent<TransformComponentSnapshotNext>(tNewEntityID) = TransformComponentSnapshotNext(*GetComponent<TransformComponentNext>(tNewEntityID));
            *GetComponent<TransformComponentSnapshotPrevious>(tNewEntityID) =
                TransformComponentSnapshotPrevious(*GetComponent<TransformComponentNext>(tNewEntityID));

            CreateComponents(tNewEntityID, p_blueprintID);

            return tNewEntityID;
        }
        EntityID EntityFactory::CreateEntity(Blueprints p_blueprintID, DirectX::XMFLOAT3 p_position, DirectX::XMFLOAT4 p_orientation, DirectX::XMFLOAT3 p_scale)
        {
            EntityManager* tEntityManager = tEntityManager->GetInstance();
            EntityID tNewEntityID = tEntityManager->AddEntity();

            TransformComponent* transComp = GetComponent<TransformComponent>(tNewEntityID);
            transComp->position = p_position;
            transComp->rotation = p_orientation;
            transComp->scale = p_scale;

            memcpy(GetComponent<TransformComponentNext>(tNewEntityID), transComp, sizeof(TransformComponent));
            memcpy(GetComponent<TransformComponentPrevious>(tNewEntityID), transComp, sizeof(TransformComponent));
            *GetComponent<TransformComponentSnapshotNext>(tNewEntityID) = TransformComponentSnapshotNext(*GetComponent<TransformComponentNext>(tNewEntityID));
            *GetComponent<TransformComponentSnapshotPrevious>(tNewEntityID) =
                TransformComponentSnapshotPrevious(*GetComponent<TransformComponentNext>(tNewEntityID));

            CreateComponents(tNewEntityID, p_blueprintID);

            return tNewEntityID;
        }
    }
}