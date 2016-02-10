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

/// DEBUG physics TODOJB remove
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>

#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

/// Standard
#include <sstream>
#include <fstream>

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
                LoadTriggers();
            }
            else
            {
                // TODOXX DO something here?
            }
        }

        bool LevelLoaderServer::BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer)
        {
            bool r_shouldBuildPhysics = true;

            const ObjectCouplingInfo& meshCoupling = m_meshCoupling[p_meshCouplingID];
            // Adds transform components to the world
            EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::Transform);
            TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityId);

            transComp->position = m_transforms[meshCoupling.transformName].translation;
            transComp->rotation = m_transforms[meshCoupling.transformName].rotation;
            transComp->scale = m_transforms[meshCoupling.transformName].scale;


            DoremiEditor::Core::TransformData transformationData = m_transforms[meshCoupling.transformName];

            if(transformationData.attributes.isAIground)
            {
                // Should build a potential field around this mesh
                CreatePotentialfieldAroundMesh(p_vertexBuffer, transformationData);
            }
            else
            {
                // Adds potential field components to the world that is not AI ground
                EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::PotentialField);
                PotentialFieldComponent* potComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(p_entityId);
                potComp->ChargedActor = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(transComp->position, -1, 2, true,
                                                                                                                  DoremiEngine::AI::AIActorType::Wall); // TODOKO hardcoded shiet
            }
            if(transformationData.attributes.isSpawner)
            {
                r_shouldBuildPhysics = false;

                EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::EntitySpawner);
                EntitySpawnComponent* entitySpawnComp = EntityHandler::GetInstance().GetComponentFromStorage<EntitySpawnComponent>(p_entityId);
                entitySpawnComp->entityBlueprint = Blueprints::EnemyEntity; //(Blueprints)transformationData.attributes.typeBlueprint;
                entitySpawnComp->maxNumSpawnedEntites = transformationData.attributes.spawnMax;
                entitySpawnComp->type = SpawnerType::TimedSpawner;
                entitySpawnComp->timeBetweenSpawns = 10;
                entitySpawnComp->spawnRadius = 100;
            }
            // If spawnpoint
            if(transformationData.attributes.spawnPointID > -1)
            {
                r_shouldBuildPhysics = false;

                // If start point, we also set this as starting respawn
                if(transformationData.attributes.startOrEndPoint == 1)
                {
                    PlayerSpawnerHandler::GetInstance()->SetCurrentSpawner(p_entityId);
                }
                // Add component
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::Trigger));

                // Set spawn point values
                TriggerComponent* t_triggerComp = GetComponent<TriggerComponent>(p_entityId);
                t_triggerComp->triggerType = TriggerType::NewSpawnPointTrigger;

                XMFLOAT3 centerPoint, minPoint, maxPoint;
                // calulate aab
                CalculateAABBBoundingBox(p_vertexBuffer, transformationData, maxPoint, minPoint, centerPoint);

                XMFLOAT3 dimension = XMFLOAT3(abs(minPoint.x - maxPoint.x) / 2.0f, abs(minPoint.y - maxPoint.y) / 2.0f, abs(minPoint.z - maxPoint.z) / 2.0f);

                //// Create material
                int materialTriggID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.0f, 0.0f, 0.0f);

                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(p_entityId, centerPoint, XMFLOAT4(0, 0, 0, 1), dimension, materialTriggID);
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(p_entityId, true);
            }
            // If endpoint
            if(transformationData.attributes.startOrEndPoint == 2)
            {
                r_shouldBuildPhysics = false;

                // Add component
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::Trigger));

                // Set spawn point values
                TriggerComponent* t_triggerComp = GetComponent<TriggerComponent>(p_entityId);
                t_triggerComp->triggerType = TriggerType::GoalTrigger;

                XMFLOAT3 centerPoint, minPoint, maxPoint;
                // calulate aab
                CalculateAABBBoundingBox(p_vertexBuffer, transformationData, maxPoint, minPoint, centerPoint);

                XMFLOAT3 dimension = XMFLOAT3(abs(minPoint.x - maxPoint.x) / 2.0f, abs(minPoint.y - maxPoint.y) / 2.0f, abs(minPoint.z - maxPoint.z) / 2.0f);

                //// Create material
                int materialTriggID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.0f, 0.0f, 0.0f);

                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(p_entityId, centerPoint, XMFLOAT4(0, 0, 0, 1), dimension, materialTriggID);
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(p_entityId, true);
            }
            if(transformationData.attributes.frequencyAffected)
            {
                // Add component
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::FrequencyAffected));

                PlatformPatrolComponent* platComp = GetComponent<PlatformPatrolComponent>(p_entityId);
                platComp->startPosition = transformationData.attributes.interactableStartPos;
                platComp->endPosition = transformationData.attributes.interactableEndPos;
            }

            return r_shouldBuildPhysics;
        }

        void LevelLoaderServer::CalculateAABBBoundingBox(const std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer,
                                                         const DoremiEditor::Core::TransformData& p_transformationData, DirectX::XMFLOAT3& o_max,
                                                         DirectX::XMFLOAT3& o_min, DirectX::XMFLOAT3& o_center)
        {
            DirectX::XMFLOAT3 maxPosition =
                DirectX::XMFLOAT3(-100000, -100000, -100000); // Hard coded low maxpos value TODOXX dangerous if maps is outside this scope...
            DirectX::XMFLOAT3 minPosition = DirectX::XMFLOAT3(100000, 100000, 100000);
            size_t length = p_vertexBuffer.size();
            for(size_t i = 0; i < length; i++)
            {
                // Finding max value
                if(p_vertexBuffer[i].position.x > maxPosition.x)
                {
                    maxPosition.x = p_vertexBuffer[i].position.x;
                }
                if(p_vertexBuffer[i].position.y > maxPosition.y)
                {
                    maxPosition.y = p_vertexBuffer[i].position.y;
                }
                if(p_vertexBuffer[i].position.z > maxPosition.z)
                {
                    maxPosition.z = p_vertexBuffer[i].position.z;
                }

                // FInding min value
                if(p_vertexBuffer[i].position.x < minPosition.x)
                {
                    minPosition.x = p_vertexBuffer[i].position.x;
                }
                if(p_vertexBuffer[i].position.y < minPosition.y)
                {
                    minPosition.y = p_vertexBuffer[i].position.y;
                }
                if(p_vertexBuffer[i].position.z < minPosition.z)
                {
                    minPosition.z = p_vertexBuffer[i].position.z;
                }
            }
            // Max and min are now centered around origo with no scale and no rotation...
            DirectX::XMVECTOR maxVector = XMLoadFloat3(&maxPosition);
            DirectX::XMVECTOR minVector = XMLoadFloat3(&minPosition);
            DirectX::XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&p_transformationData.rotation));
            DirectX::XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&p_transformationData.translation));
            DirectX::XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&p_transformationData.scale));
            maxVector = XMVector3Transform(maxVector, scale);
            maxVector = XMVector3Transform(maxVector, translation);

            minVector = XMVector3Transform(minVector, scale);
            minVector = XMVector3Transform(minVector, translation);
            // minVector = XMVector3Transform(minVector, translation * rotation * scale);
            // maxVector = XMVector3Rotate(maxVector, XMLoadFloat4(&p_transformationData.rotation));
            DirectX::XMFLOAT3 centerPoint;
            XMStoreFloat3(&o_center, (maxVector + minVector) / 2);
            XMStoreFloat3(&o_max, maxVector);
            XMStoreFloat3(&o_min, minVector);
        }

        void LevelLoaderServer::CreatePotentialfieldAroundMesh(const std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer,
                                                               const DoremiEditor::Core::TransformData& p_transformationData)
        {
            using namespace DirectX;
            XMFLOAT3 centerPoint, minPoint, maxPoint;
            CalculateAABBBoundingBox(p_vertexBuffer, p_transformationData, maxPoint, minPoint, centerPoint);

            centerPoint.y = maxPoint.y;
            PotentialFieldGridCreator t_gridCreator = PotentialFieldGridCreator(m_sharedContext);
            DoremiEngine::AI::PotentialField* field =
                m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewField(maxPoint.x - minPoint.x, maxPoint.z - minPoint.z, 1, 1, centerPoint);
            t_gridCreator.BuildGridUsingPhysicXAndGrid(field);
        }
    }
}
