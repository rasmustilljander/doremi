/// Game side
#include <LevelLoaderServer.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityFactory.hpp>
#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>

// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/EntitySpawnerComponent.hpp>
#include <EntityComponent/Components/PlatformPatrolComponent.hpp>
#include <EntityComponent/Components/NetworkObjectComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/CharacterControlComponen.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>

/// Engine side
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>

// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
// Debug?
#include <PotentialFieldGridCreator.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

// Physics
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

/// DEBUG physics TODOJB remove
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>

#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

/// Standard
#include <sstream>
#include <fstream>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        LevelLoaderServer::LevelLoaderServer(const DoremiEngine::Core::SharedContext& p_sharedContext) : LevelLoader(p_sharedContext) {}

        LevelLoaderServer::~LevelLoaderServer() {}

        void LevelLoaderServer::LoadLevel(const std::string& p_fileName)
        {
            using namespace std;
            string fileName = m_sharedContext.GetWorkingDirectory() + p_fileName;
            ifstream ifs;
            ifs.open(fileName, ifstream::in | ofstream::binary);
            if(ifs.is_open() == true)
            {
                // testa o läsa lite

                // scene name
                int sceneNameSize;
                ifs.read((char*)&sceneNameSize, sizeof(int));
                char* sceneName = new char[sceneNameSize];
                ifs.read((char*)sceneName, sizeof(char) * sceneNameSize);

                // how much different stuff there is
                int nrMats, nrTransforms, nrMeshes, nrLights;
                ifs.read((char*)&nrMats, sizeof(int));
                ifs.read((char*)&nrTransforms, sizeof(int));
                ifs.read((char*)&nrMeshes, sizeof(int));
                ifs.read((char*)&nrLights, sizeof(int));

                LoadMaterial(ifs, nrMats);
                LoadTransforms(ifs, nrTransforms);
                LoadMeshes(ifs, nrMeshes);
                LoadLights(ifs, nrLights);
                BuildEntities();
                std::vector<DoremiEngine::AI::PotentialField*>& potentialFields =
                    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().GetAllActiveFields();
                size_t length = potentialFields.size();
                PotentialFieldGridCreator t_gridCreator = PotentialFieldGridCreator(m_sharedContext);
                for(size_t i = 0; i < length; i++)
                {
                    t_gridCreator.BuildGridUsingPhysicXAndGrid(potentialFields[i]);
                }
                LoadTriggers();
            }
            else
            {
                // TODOXX DO something here?
            }
        }

        bool LevelLoaderServer::BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer)
        {
            bool r_shouldCookStaticPhysics = true;
            bool t_builtPhysics = false;

            const ObjectCouplingInfo& meshCoupling = m_meshCoupling[p_meshCouplingID];
            DoremiEditor::Core::TransformData transformationData = m_transforms[meshCoupling.transformName];

            // Adds transform components to the world
            EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::Transform);
            TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityId);
            transComp->position = m_transforms[meshCoupling.transformName].translation;
            transComp->rotation = m_transforms[meshCoupling.transformName].rotation;
            transComp->scale = m_transforms[meshCoupling.transformName].scale;


            // If we are AI ground
            if(transformationData.attributes.isAIground)
            {
                // Should build a potential field around this mesh
                using namespace DirectX;
                // Get the aabb box around the mesh with a AIGround attribute
                XMFLOAT3 centerPoint, minPoint, maxPoint;
                CalculateAABBBoundingBox(p_vertexBuffer, transformationData, maxPoint, minPoint, centerPoint);

                // Set it to the top of the mesh since we bassicly want a 2d field offset to a Y value
                centerPoint.y = maxPoint.y;
                // Create a new field with width and height calculated from bounding box.
                // TODOCONFIG 50, 50 is hardcoded how many quads in x and z. should be calculated from a given quad size instead
                DoremiEngine::AI::PotentialField* field =
                    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewField(maxPoint.x - minPoint.x, maxPoint.z - minPoint.z, 1, 1, centerPoint);
                if(!transformationData.attributes.isStatic)
                {
                    EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::PotentialField);
                    PotentialFieldComponent* potComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(p_entityId);
                    potComp->Field = field;
                    potComp->isField = true;
                }
            }
            else if(transformationData.attributes.isPotentialFieldCollidable)
            {
                // Adds potential field components to the world that is not AI ground
                EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::PotentialField);
                PotentialFieldComponent* potComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(p_entityId);
                potComp->ChargedActor = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(transComp->position, -1, 2, true,
                                                                                                                  DoremiEngine::AI::AIActorType::Wall); // TODOCONFIG hardcoded shiet
            }

            // If a spawner, mostly enemy spawners
            if(transformationData.attributes.isSpawner)
            {
                r_shouldCookStaticPhysics = false;

                EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::EntitySpawner);
                EntitySpawnComponent* entitySpawnComp = EntityHandler::GetInstance().GetComponentFromStorage<EntitySpawnComponent>(p_entityId);
                entitySpawnComp->entityBlueprint = static_cast<Blueprints>(transformationData.attributes.spawnTypeBlueprint);
                entitySpawnComp->maxNumSpawnedEntites = 1;
                entitySpawnComp->maxAlive = transformationData.attributes.spawnMaxAlive;
                entitySpawnComp->type = SpawnerType::TimedSpawner;
                entitySpawnComp->timeBetweenSpawns = transformationData.attributes.spawnFrequency;
                entitySpawnComp->spawnRadius = 10000;
            }

            float DEBUGoffset = 0;
            // If spawnpoint, set physic properties
            if(transformationData.attributes.spawnPointID > -1)
            {

                DEBUGoffset = 200;
                r_shouldCookStaticPhysics = false;

                // If start point, we also set this as starting respawn
                if(transformationData.attributes.startOrEndPoint == 1)
                {
                    PlayerSpawnerHandler::GetInstance()->SetCurrentSpawner(p_entityId);
                }
                // Add component
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::Trigger) |
                                                                          static_cast<uint32_t>(ComponentType::RigidBody) |
                                                                          static_cast<uint32_t>(ComponentType::PhysicalMaterial));

                // Set spawn point values
                TriggerComponent* t_triggerComp = GetComponent<TriggerComponent>(p_entityId);
                t_triggerComp->triggerType = TriggerType::NewSpawnPointTrigger;

                RigidBodyComponent* t_rigidBody = GetComponent<RigidBodyComponent>(p_entityId);
                t_rigidBody->flags = RigidBodyFlags::trigger;
                t_rigidBody->geometry = RigidBodyGeometry::staticBox;
            }
            // If endpoint
            if(transformationData.attributes.startOrEndPoint == 2)
            {
                r_shouldCookStaticPhysics = false;

                // Add components for end point
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::Trigger) |
                                                                          static_cast<uint32_t>(ComponentType::RigidBody) |
                                                                          static_cast<uint32_t>(ComponentType::PhysicalMaterial));

                // Set end point values
                TriggerComponent* t_triggerComp = GetComponent<TriggerComponent>(p_entityId);
                t_triggerComp->triggerType = TriggerType::GoalTrigger;

                RigidBodyComponent* t_rigidBody = GetComponent<RigidBodyComponent>(p_entityId);
                t_rigidBody->flags = RigidBodyFlags::trigger;
                t_rigidBody->geometry = RigidBodyGeometry::staticBox;
            }

            if(transformationData.attributes.frequencyAffected)
            {
                r_shouldCookStaticPhysics = false;
                ///////////////////////////////////////////////////////////// IF YOU ADD THIS CODE THE POINTERS OF PHYSICS MODULE WILL SWAP FOR SOME
                // WIERD REASON //////////////////////////////////////////////////////////////
                // Add component
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::FrequencyAffected) |
                                                                          static_cast<uint32_t>(ComponentType::PlatFormPatrolComponent) |
                                                                          static_cast<uint32_t>(ComponentType::NetworkObject) |
                                                                          static_cast<uint32_t>(ComponentType::RigidBody) |
                                                                          static_cast<uint32_t>(ComponentType::PhysicalMaterial) |
                                                                          static_cast<uint32_t>(ComponentType::Range));

                // add platform
                PlatformPatrolComponent* platComp = GetComponent<PlatformPatrolComponent>(p_entityId);
                platComp->startPosition = transformationData.attributes.interactableStartPos;
                platComp->endPosition = transformationData.attributes.interactableEndPos;
                platComp->startPosition.z *= -1.0f;
                platComp->endPosition.z *= -1.0f;


                // Physical material comp
                PhysicsMaterialComponent* t_physMatComp = GetComponent<PhysicsMaterialComponent>(p_entityId);
                t_physMatComp->p_materialID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);


                // Rigid body comp
                RigidBodyComponent* t_rigidBodyComp = GetComponent<RigidBodyComponent>(p_entityId);

                // t_rigidBodyComp->boxDims = dimension;
                t_rigidBodyComp->flags =
                    static_cast<RigidBodyFlags>(static_cast<uint32_t>(RigidBodyFlags::kinematic) | static_cast<uint32_t>(RigidBodyFlags::drain));
                t_rigidBodyComp->geometry = RigidBodyGeometry::dynamicBox;

                // Add net comp
                NetworkObjectComponent* netComp = GetComponent<NetworkObjectComponent>(p_entityId);
                *netComp = NetworkObjectComponent(1);

                // Add range component
                RangeComponent* t_rangeComp = GetComponent<RangeComponent>(p_entityId);
                t_rangeComp->range = transformationData.attributes.interactableRange;
            }
            // WARNING need to be after all others to check against what kind of rigid body it is
            if(transformationData.attributes.isDangerous)
            {
                // check if we're character controlled or static death
                if(EntityHandler::GetInstance().HasComponents(p_entityId, static_cast<uint32_t>(ComponentType::RigidBody)))
                {
                    // Rigid body comp
                    RigidBodyComponent* t_rigidBodyComp = GetComponent<RigidBodyComponent>(p_entityId);
                    if(t_rigidBodyComp->geometry == RigidBodyGeometry::dynamicBox)
                    {
                        // We don't know how to do this yet...
                    }
                    else if(t_rigidBodyComp->geometry == RigidBodyGeometry::staticBox)
                    {
                        // Ok already cool data here
                    }
                    else
                    {
                        std::cout << "Code missing for dangerous object that are not static or dynamic box" << std::endl;
                    }
                }
                else
                {
                    r_shouldCookStaticPhysics = false;
                    // Add components for end point
                    EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::Trigger) |
                                                                              static_cast<uint32_t>(ComponentType::RigidBody) |
                                                                              static_cast<uint32_t>(ComponentType::PhysicalMaterial));

                    // Set end point values
                    TriggerComponent* t_triggerComp = GetComponent<TriggerComponent>(p_entityId);
                    t_triggerComp->triggerType = TriggerType::DeathTrigger;

                    RigidBodyComponent* t_rigidBody = GetComponent<RigidBodyComponent>(p_entityId);
                    t_rigidBody->flags = RigidBodyFlags::trigger;
                    t_rigidBody->geometry = RigidBodyGeometry::staticBox;
                }
            }
            if(transformationData.attributes.checkPointID > -1)
            {
                r_shouldCookStaticPhysics = false;
            }


            // If we're rigid body
            if(EntityHandler::GetInstance().HasComponents(p_entityId, static_cast<uint32_t>(ComponentType::RigidBody)))
            {
                // Get us our rigid body manager
                DoremiEngine::Physics::RigidBodyManager& rigidBodyManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

                // calulate aab
                XMFLOAT3 centerPoint, minPoint, maxPoint;
                CalculateAABBBoundingBox(p_vertexBuffer, transformationData, maxPoint, minPoint, centerPoint);

                XMFLOAT3 dimension = XMFLOAT3(abs(minPoint.x - maxPoint.x) / 2.0f, abs(minPoint.y - maxPoint.y) / 2.0f, abs(minPoint.z - maxPoint.z) / 2.0f);
                RigidBodyComponent* bodyComp = GetComponent<RigidBodyComponent>(p_entityId);
                bodyComp->boxDims = dimension;

                int materialTriggID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.0f, 0.0f, 0.0f);

                // Get the material. This is haxxy. It probably works most of the time
                PhysicsMaterialComponent* matComp = GetComponent<PhysicsMaterialComponent>(p_entityId);
                matComp->p_materialID = materialTriggID;

                switch(bodyComp->geometry)
                {
                    case RigidBodyGeometry::dynamicBox:
                        rigidBodyManager.AddBoxBodyDynamic(p_entityId, transComp->position, XMFLOAT4(0, 0, 0, 1), bodyComp->boxDims, matComp->p_materialID);
                        break;
                    case RigidBodyGeometry::dynamicSphere:
                        rigidBodyManager.AddSphereBodyDynamic(p_entityId, transComp->position, bodyComp->radius);
                        break;
                    case RigidBodyGeometry::dynamicCapsule:
                        rigidBodyManager.AddCapsuleBodyDynamic(p_entityId, transComp->position, XMFLOAT4(0, 0, 0, 1), bodyComp->height, bodyComp->radius);
                        break;
                    case RigidBodyGeometry::staticBox:
                        transComp->position.y += DEBUGoffset;
                        rigidBodyManager.AddBoxBodyStatic(p_entityId, transComp->position, XMFLOAT4(0, 0, 0, 1), bodyComp->boxDims, matComp->p_materialID);
                        break;
                    default:
                        break;
                }
                // Apply flags
                if(((int)bodyComp->flags & (int)RigidBodyFlags::kinematic) == (int)RigidBodyFlags::kinematic)
                {
                    rigidBodyManager.SetKinematicActor(p_entityId, true);

                    // TODOJB TODOXX haxy callback filtering. Read comment in LevelLoader::SetPhysicalAttributesOnMesh
                    rigidBodyManager.SetCallbackFiltering(p_entityId, 3, 0, 0, 2);
                }
                if(((int)bodyComp->flags & (int)RigidBodyFlags::trigger) == (int)RigidBodyFlags::trigger)
                {
                    rigidBodyManager.SetTrigger(p_entityId, true);
                }
                if(((int)bodyComp->flags & (int)RigidBodyFlags::drain) == (int)RigidBodyFlags::drain)
                {
                    rigidBodyManager.SetDrain(p_entityId, true);
                }
                if(((int)bodyComp->flags & (int)RigidBodyFlags::ignoredDEBUG) == (int)RigidBodyFlags::ignoredDEBUG)
                {
                    rigidBodyManager.SetIgnoredDEBUG(p_entityId);
                }
            }


            return r_shouldCookStaticPhysics;
        }
    }
}
