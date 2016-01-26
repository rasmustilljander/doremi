/// Game side
#include <LevelLoaderServer.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityFactory.hpp>
// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
/// Engine side
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>

// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>

// Timing
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

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
        LevelLoaderServer::LevelLoaderServer(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
        LevelLoaderServer::~LevelLoaderServer() {}
        void LevelLoaderServer::LoadLevel(const std::string& p_fileName)
        {
            using namespace std;
            using namespace DirectX;
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

                // ladda material
                for(int i = 1; i < nrMats; i++) // defualt material, så kör inte hela nrMats
                {
                    int materialNameSize;
                    ifs.read((char*)&materialNameSize, sizeof(int));
                    char* materialName = new char[materialNameSize];
                    ifs.read((char*)materialName, sizeof(char) * materialNameSize);

                    MaterialData materialData;
                    ifs.read((char*)&materialData.mapMasks, sizeof(int));
                    ifs.read((char*)&materialData.diffuse, sizeof(float));
                    ifs.read((char*)&materialData.color, sizeof(float) * 3);
                    ifs.read((char*)&materialData.ambColor, sizeof(float) * 3);
                    ifs.read((char*)&materialData.specColor, sizeof(float) * 3);
                    ifs.read((char*)&materialData.specCosine, sizeof(float));
                    ifs.read((char*)&materialData.specEccentricity, sizeof(float));
                    ifs.read((char*)&materialData.specRollOff, sizeof(float));

                    int diffuseTextureNameSize;
                    ifs.read((char*)&diffuseTextureNameSize, sizeof(int));
                    char* diffuseTextureName = new char[diffuseTextureNameSize];
                    ifs.read((char*)diffuseTextureName, sizeof(char) * diffuseTextureNameSize);
                }
                // ladda transforms
                for(int i = 0; i < nrTransforms; i++)
                {
                    int parentNameSize;
                    int transformNameSize;

                    ifs.read((char*)&parentNameSize, sizeof(int));
                    ifs.read((char*)&transformNameSize, sizeof(int));

                    char* parentName = new char[parentNameSize]; // TODOKO Not supported yet
                    char* transformName = new char[transformNameSize];

                    ifs.read((char*)parentName, sizeof(char) * parentNameSize);
                    ifs.read((char*)transformName, sizeof(char) * transformNameSize);
                    TransformData transformData;

                    ifs.read((char*)&transformData.pos, sizeof(XMFLOAT3));
                    ifs.read((char*)&transformData.rot, sizeof(float) * 4); // quaternion tror jag
                    ifs.read((char*)&transformData.scale, sizeof(float) * 3);
                    ifs.read((char*)&transformData.attributes, sizeof(CustomAttributes));

                    m_transforms[transformName] = transformData;
                }
                // ladda meshes. TODOSH Fixa så samma mesh itne läses in flera gånger, alltså så att samma mesh data inte finns på 2 ställen på GPU
                for(int i = 0; i < nrMeshes; i++)
                {
                    int transformNameSize;
                    int meshNameSize;
                    int nrOfTransforms;

                    ifs.read((char*)&nrOfTransforms, sizeof(int));
                    std::vector<char*> transformNames;
                    char* transformName = nullptr;

                    for (int t = 0; t < nrOfTransforms; t++) //läser in alla transforms för meshen, blir flera om instanciering skall användas
                    {
                        ifs.read((char*)&transformNameSize, sizeof(int));
                        transformName = new char[transformNameSize];

                        ifs.read((char*)transformName, sizeof(char) * transformNameSize);
                        transformNames.push_back(transformName);
                    }


                    ifs.read((char*)&meshNameSize, sizeof(int));
                    char* meshName = new char[meshNameSize];

                    ifs.read((char*)meshName, sizeof(char) * meshNameSize);


                    int materialNameSize;
                    ifs.read((char*)&materialNameSize, sizeof(int));

                    char* materialName = new char[materialNameSize];
                    ifs.read((char*)materialName, sizeof(char) * materialNameSize);


                    /////DEBUG FÖR DEMO///////
                    if (meshNameSize == 30)
                        materialName = "AppartmentMaterial";

                    // messageSTART****
                    int meshID;
                    ifs.read((char*)&meshID, sizeof(int));

                    MeshData meshData;

                    ifs.read((char*)&meshData.nrPos, sizeof(int));
                    ifs.read((char*)&meshData.nrNor, sizeof(int));
                    ifs.read((char*)&meshData.nrUV, sizeof(int));
                    ifs.read((char*)&meshData.nrI, sizeof(int));
                    ifs.read((char*)&meshData.triangleCount, sizeof(int));

                    meshData.positions = new XMFLOAT3[meshData.nrPos];
                    meshData.normals = new XMFLOAT3[meshData.nrNor];
                    meshData.uvs = new XMFLOAT2[meshData.nrUV];

                    meshData.indexPositions = new int[meshData.nrI];
                    meshData.indexNormals = new int[meshData.nrI];
                    meshData.indexUVs = new int[meshData.nrI];
                    meshData.trianglesPerFace = new int[meshData.triangleCount];
                    vector<XMFLOAT3> poss;
                    ifs.read((char*)meshData.positions, sizeof(XMFLOAT3) * meshData.nrPos);
                    ifs.read((char*)meshData.normals, sizeof(XMFLOAT3) * meshData.nrNor);
                    ifs.read((char*)meshData.uvs, sizeof(XMFLOAT2) * meshData.nrUV);

                    ifs.read((char*)meshData.indexPositions, sizeof(int) * meshData.nrI);
                    ifs.read((char*)meshData.indexNormals, sizeof(int) * meshData.nrI);
                    ifs.read((char*)meshData.indexUVs, sizeof(int) * meshData.nrI);
                    ifs.read((char*)meshData.trianglesPerFace, sizeof(int) * meshData.triangleCount);

                    m_meshes[meshName] = meshData;
                    for (size_t i = 0; i < nrOfTransforms; i++)
                    {
                        m_meshCoupling.push_back(ObjectCouplingInfo(transformNames[i], meshName, materialName));
                    }
                }

                for(int i = 0; i < nrLights; i++)
                {
                    int transformNameSize;
                    int lightNameSize;

                    ifs.read((char*)&transformNameSize, sizeof(int));
                    ifs.read((char*)&lightNameSize, sizeof(int));

                    char* transformName = new char[transformNameSize];
                    char* lightName = new char[lightNameSize];

                    ifs.read((char*)transformName, sizeof(char) * transformNameSize);
                    ifs.read((char*)lightName, sizeof(char) * lightNameSize);

                    LightData lightData;

                    ifs.read((char*)&lightData.type, sizeof(int));
                    ifs.read((char*)&lightData.decayType, sizeof(int));
                    ifs.read((char*)&lightData.intensity, sizeof(float));
                    ifs.read((char*)&lightData.colorDiffuse, sizeof(XMFLOAT3));
                    ifs.read((char*)&lightData.direction, sizeof(XMFLOAT3));
                    ifs.read((char*)&lightData.dropOff, sizeof(float));
                    ifs.read((char*)&lightData.coneAngle, sizeof(float));
                    ifs.read((char*)&lightData.penumAgle, sizeof(float));
                }
            }
            // Creating the entities
            size_t length = m_meshCoupling.size();
            for(size_t i = 0; i < length; i++)
            {
                std::string transformName = m_meshCoupling[i].transformName;
                std::string meshName = m_meshCoupling[i].meshName;
                int entityID = EntityFactory::GetInstance()->CreateEntity(Blueprints::EmptyEntity);
                EntityHandler::GetInstance().AddComponent(entityID, (int)ComponentType::Transform);
                TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);

                transComp->position = m_transforms[transformName].pos;
                transComp->rotation = m_transforms[transformName].rot;
                transComp->scale = m_transforms[transformName].scale;

                EntityHandler::GetInstance().AddComponent(entityID, (int)ComponentType::PotentialField);
                PotentialFieldComponent* pfComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(entityID);
                pfComp->ChargedActor = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(transComp->position, -3, 3, true); // TODOKO hardcoded shiet
                // TODO Make it finer!
                m_entityID = entityID;
                m_currentScale = m_transforms[transformName].scale;
                m_currentOrientation = m_transforms[transformName].rot;
                m_currentPos = m_transforms[transformName].pos;
                XMVECTOR realPos = XMLoadFloat3(&m_currentPos);
                XMStoreFloat3(&m_currentPos, realPos);
                BuildMesh(m_meshes[m_meshCoupling[i].meshName]);
                // EntityHandler::GetInstance().AddComponent(entityID, (int)ComponentType::RigidBody);
                // RigidBodyComponent* rbComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityID);
                // rbComp->p_bodyID = m_entityID;
            }
            // Triggering ;)
            int entityIDTrigger = EntityFactory::GetInstance()->CreateEntity(Blueprints::TriggerEntity);
            EntityHandler::GetInstance().AddComponent(entityIDTrigger, (int)ComponentType::Trigger | (int)ComponentType::Transform | (int)ComponentType::RigidBody);
            TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityIDTrigger);
            transComp->position = XMFLOAT3(-420.4f, 151.5f, -110.3f);
            transComp->rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            TriggerComponent* triggComp = EntityHandler::GetInstance().GetComponentFromStorage<TriggerComponent>(entityIDTrigger);
            triggComp->dimensions = XMFLOAT3(7.0f, 10.0f, 34.0f);
            triggComp->triggerType = TriggerType::GoalTrigger;
            RigidBodyComponent* rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityIDTrigger);
            int materialTriggID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.0f, 0.0f, 0.0f);
            rigidComp->p_bodyID =
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(entityIDTrigger, transComp->position, transComp->rotation,
                                                                                          triggComp->dimensions, materialTriggID);
            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(rigidComp->p_bodyID, true);

            // int entityIDTrigger2 = EntityFactory::GetInstance()->CreateEntity(Blueprints::TriggerEntity); // TODOEA REMOVE
            // EntityHandler::GetInstance().AddComponent(entityIDTrigger2, (int)ComponentType::Trigger | (int)ComponentType::Transform |
            // (int)ComponentType::RigidBody);
            // TransformComponent* transComp2 = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityIDTrigger2);
            // transComp2->position = XMFLOAT3(0, 0, 0);
            // transComp2->rotation = XMFLOAT4(0, 0, 0, 1);
            // TriggerComponent* triggComp2 = EntityHandler::GetInstance().GetComponentFromStorage<TriggerComponent>(entityIDTrigger2);
            // triggComp2->dimensions = XMFLOAT3(50, 50, 50);
            // triggComp2->triggerType = TriggerType::NoTrigger;
            // RigidBodyComponent* rigidComp2 = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(entityIDTrigger2);
            // int materialTriggID2 = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
            // rigidComp2->p_bodyID = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic(entityIDTrigger2,
            // transComp2->position, transComp2->rotation, triggComp2->dimensions, materialTriggID2);
            // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetTrigger(rigidComp2->p_bodyID, true);
        }
        std::vector<DoremiEngine::Graphic::Vertex> LevelLoaderServer::BuildMesh(const MeshData& p_data)
        {
            using namespace std;
            vector<DoremiEngine::Graphic::Vertex> vertexBuffer;

            // Hax for collision meshes
            XMMATRIX scaleMat = XMMatrixScalingFromVector(XMLoadFloat3(&m_currentScale));
            vector<XMFLOAT3> positionsPX;
            vector<int> indicesPX;
            // END HAX

            int nrVertices = p_data.nrNor;

            DoremiEngine::Graphic::Vertex tempV;

            for(int i = 0; i < p_data.nrI; i = i + 3)
            {


                tempV.position = p_data.positions[p_data.indexPositions[i + 2]];
                tempV.position.z = tempV.position.z * -1.0f;
                tempV.normal = p_data.normals[p_data.indexNormals[i + 2]];
                tempV.normal.z = tempV.normal.z * -1.0f;
                tempV.textureCoordinate = p_data.uvs[p_data.indexUVs[i + 2]];
                tempV.textureCoordinate.y = tempV.textureCoordinate.y - 1.0f;

                vertexBuffer.push_back(tempV);
                // HAX
                XMVECTOR posVec = XMLoadFloat3(&tempV.position);
                posVec = XMVector3Transform(posVec, scaleMat);
                XMFLOAT3 scaledPos;
                XMStoreFloat3(&scaledPos, posVec);
                positionsPX.push_back(scaledPos);
                indicesPX.push_back(i);
                // END HAX

                tempV.position = p_data.positions[p_data.indexPositions[i + 1]];
                tempV.position.z = tempV.position.z * -1.0f;
                tempV.normal = p_data.normals[p_data.indexNormals[i + 1]];
                tempV.normal.z = tempV.normal.z * -1.0f;
                tempV.textureCoordinate = p_data.uvs[p_data.indexUVs[i + 1]];
                tempV.textureCoordinate.y = tempV.textureCoordinate.y - 1.0f;

                vertexBuffer.push_back(tempV);
                // HAX
                posVec = XMLoadFloat3(&tempV.position);
                posVec = XMVector3Transform(posVec, scaleMat);
                scaledPos;
                XMStoreFloat3(&scaledPos, posVec);
                positionsPX.push_back(scaledPos);
                indicesPX.push_back(i + 1);
                // END HAX

                tempV.position = p_data.positions[p_data.indexPositions[i]];
                tempV.position.z = tempV.position.z * -1.0f;
                tempV.normal = p_data.normals[p_data.indexNormals[i]];
                tempV.normal.z = tempV.normal.z * -1.0f;
                tempV.textureCoordinate = p_data.uvs[p_data.indexUVs[i]];
                tempV.textureCoordinate.y = tempV.textureCoordinate.y - 1.0f;

                vertexBuffer.push_back(tempV);
                // HAX
                posVec = XMLoadFloat3(&tempV.position);
                posVec = XMVector3Transform(posVec, scaleMat);
                scaledPos;
                XMStoreFloat3(&scaledPos, posVec);
                positionsPX.push_back(scaledPos);
                indicesPX.push_back(i + 2);
                // END HAX
            }
            // HAX
            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddMeshBodyStatic(
                m_entityID, m_currentPos, m_currentOrientation, positionsPX, indicesPX,
                m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.5, 0.5, 0.5));
            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetDrain(m_entityID, true);
            // END HAX
            return vertexBuffer;
        }
    }
}
