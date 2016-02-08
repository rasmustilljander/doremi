/// Game side
#include <LevelLoaderClient.hpp>
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
/// Engine side
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/LightManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Animation/SkeletalInformation.hpp>
// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>
// Potential Field
#include <Doremi/Core/Include/PotentialFieldGridCreator.hpp>
// Physics

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
        LevelLoaderClient::LevelLoaderClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : LevelLoader(p_sharedContext) {}

        LevelLoaderClient::~LevelLoaderClient() {}

        void LevelLoaderClient::LoadLevel(const std::string& p_fileName)
        {
            // TODOXX WTF, this function must be restructured.
            using namespace std;
            const string fileName = m_sharedContext.GetWorkingDirectory() + p_fileName;
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

                BuildLights();
            }
            else
            {
                // TODOXX somethings wrong
            }
        }

        CharacterDataNames LevelLoaderClient::LoadSkeletalCharacter(const std::string& p_fileName, DoremiEngine::Graphic::SkeletalInformation& p_skeletalInformation)
        {
            string fileName = m_sharedContext.GetWorkingDirectory() + p_fileName;

            // fileName.append((string)fileNameArg.asChar());
            // fileName.append(".drm");
            ifstream ifs;
            ifs.open(fileName, ifstream::in | ifstream::binary); //ändra till ascii??
            if(ifs.is_open() == true)
            {
                int sceneNameSize;
                ifs.read((char*)&sceneNameSize, sizeof(int));
                char* sceneName = new char[sceneNameSize];
                ifs.read((char*)sceneName, sizeof(char) * sceneNameSize);
                std::string t_sceneName = sceneName;

                int nrJoints, nrMats, nrTransforms, nrMeshes;
                ifs.read((char*)&nrJoints, sizeof(int));
                ifs.read((char*)&nrMats, sizeof(int));
                ifs.read((char*)&nrTransforms, sizeof(int));
                ifs.read((char*)&nrMeshes, sizeof(int));

                // int idleAnimationStart = GetAnimationAttribute("StartFramesIdle");
                // Per skelettmesh
                std::vector<int> t_jointHeirarchy;
                std::map<std::string, DoremiEngine::Graphic::AnimationClip> t_animations;
                std::vector<std::vector<DirectX::XMFLOAT4X4>> t_boneTransformsAll;
                std::vector<std::vector<std::string>> t_transformNamesAll;

                // Per animationsclip
                DoremiEngine::Graphic::AnimationClip t_animationClip;
                std::vector<std::string> t_childTransformNames;

                // Per joint
                for(int i = 0; i < nrJoints; i++)
                {
                    int ID, parentID; // joint IDs
                    int nrChildrenTransforms;
                    int nrKeyFrames;
                    int nameSize;


                    ifs.read((char*)&ID, sizeof(int));
                    ifs.read((char*)&parentID, sizeof(int));
                    ifs.read((char*)&nrChildrenTransforms, sizeof(int));
                    ifs.read((char*)&nrKeyFrames, sizeof(int));
                    ifs.read((char*)&nameSize, sizeof(int));

                    char* name = new char[nameSize]; // namnet på jointen
                    ifs.read((char*)name, sizeof(char) * nameSize);

                    t_jointHeirarchy.push_back(parentID);
                    std::vector<std::string> t_transformNames;
                    for(int y = 0; y < nrChildrenTransforms;
                        y++) // skriv alla transforms (förutom joints) som är children till denna joint, dvs transforms till meshes
                    {
                        static int legbotCount = 0;
                        int childNameSize;
                        ifs.read((char*)&childNameSize, sizeof(int));
                        char* childTransformName = new char[childNameSize]; // namn på transform som skall vara till mesh, dvs INGEN joint
                        ifs.read((char*)childTransformName, sizeof(char) * childNameSize);
                        // Den här här svårast att få in. Det ska vara Bonetransformsen men jag måste mappa namnen på nåt vänster till vilken joint
                        // som är intressant
                        t_transformNames.push_back(childTransformName); // Kanske ska bort om den under funkar
                        std::string childName(childTransformName);
                        if(!std::strcmp(childTransformName, "LegBot1"))
                        {
                            if(legbotCount == 0)
                            {
                                legbotCount++;
                                t_childTransformNames.push_back(childTransformName); // PRovar attköra med denna Finns 2 legbot1!!
                            }
                        }
                        else
                        {
                            t_childTransformNames.push_back(childTransformName); // PRovar attköra med denna Finns 2 legbot1!!
                        }
                        delete childTransformName;
                    }
                    t_transformNamesAll.push_back(t_transformNames); // ska bort om den övre funkar

                    // SPara ner keyframesen i boneanimaiton som sedan sparas ner i animationclip å mappas mot animationclipnamnet
                    DoremiEngine::Graphic::BoneAnimation t_boneAnimation;

                    for(int y = 0; y < nrKeyFrames; y++) // skriv keyframsen
                    {
                        DoremiEngine::Graphic::KeyFrame t_keyFrameTemp;
                        int t_frame;
                        ifs.read((char*)&t_keyFrameTemp.position, sizeof(float) * 3);
                        ifs.read((char*)&t_keyFrameTemp.quaternion, sizeof(float) * 4);
                        ifs.read((char*)&t_keyFrameTemp.scale, sizeof(float) * 3);
                        // t_keyFrameTemp.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
                        ifs.read((char*)&t_frame, sizeof(int));
                        t_keyFrameTemp.position.z *= -1.0f; // test
                        // t_keyFrameTemp.quaternion.x *= -1;
                        // t_keyFrameTemp.quaternion.y *= -1;
                        // t_keyFrameTemp.quaternion.z *= -1;
                        // t_keyFrameTemp.quaternion.w *= -1;

                        float t_timeMax = 2.0f;

                        float t_currentTime = (t_timeMax / float(nrKeyFrames)) * t_frame;
                        t_keyFrameTemp.time = t_currentTime;
                        t_boneAnimation.Keyframes.push_back(t_keyFrameTemp);
                    }

                    t_animationClip.BoneAnimations.push_back(t_boneAnimation);

                    delete name; // name isnt used atm
                }

                t_animations["Run"] = t_animationClip;
                // materialen kommer senare att läsas in här imellan
                std::map<std::string, DirectX::XMFLOAT4X4> t_transformMap;
                for(int i = 0; i < nrTransforms; i++)
                {
                    int parentNameSize;
                    int transformNameSize;

                    // Read namelengths of parent(not used) and transform
                    ifs.read((char*)&parentNameSize, sizeof(int));
                    ifs.read((char*)&transformNameSize, sizeof(int));

                    // Read parent name(not used) and transformname
                    char* parentName = new char[parentNameSize]; // denna används inte alls för tillfället men kan komma att implementeras i
                    // framtiden! transformsen (för meshar) ska inte ha några parents nu. Läs ändå for
                    // safety! (y)
                    char* transformName = new char[transformNameSize];
                    ifs.read((char*)parentName, sizeof(char) * parentNameSize);
                    ifs.read((char*)transformName, sizeof(char) * transformNameSize);
                    // Convert to string. not sure how char* works properly with map if I delete them
                    std::string t_transformName(transformName);

                    // Delete the char*
                    delete parentName;
                    delete transformName;

                    // Fetch the data we are interested in
                    TransformData transformDataTemp;
                    ifs.read((char*)&transformDataTemp.pos, sizeof(float) * 3);
                    ifs.read((char*)&transformDataTemp.rot, sizeof(float) * 4);
                    ifs.read((char*)&transformDataTemp.scale, sizeof(float) * 3);
                    // transformDataTemp.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
                    // Make a matrix from it and store it as XMFLOAT4x4
                    transformDataTemp.pos.z *= -1.0f;
                    // transformDataTemp.rot.w *= -1;
                    // transformDataTemp.rot.x *= -1;
                    // transformDataTemp.rot.y *= -1;
                    // transformDataTemp.rot.z *= -1;
                    XMVECTOR t_translation = XMLoadFloat3(&transformDataTemp.pos);
                    XMVECTOR t_quaternion = XMLoadFloat4(&transformDataTemp.rot);
                    XMVECTOR t_scale = XMLoadFloat3(&transformDataTemp.scale);
                    XMVECTOR t_zero = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));

                    XMFLOAT4X4 t_transformationMatrix;
                    XMStoreFloat4x4(&t_transformationMatrix, XMMatrixAffineTransformation(t_scale, t_zero, t_quaternion, t_translation));
                    // Push the name to the map with the value of the matrix (XMFLAOT4x4)
                    t_transformMap[t_transformName] = t_transformationMatrix;
                    // t_transformMap[t_childTransformNames[16 -i]] = t_transformationMatrix; //test

                    //// Fråga simon! Nrtransforms = 17 men childtransformnames = 18
                }
                // This buffer is used to combine all the meshes into one. And convert the vertexstruct to skeletalvertex from the computevertex thing
                vector<DoremiEngine::Graphic::SkeletalVertex> t_skeletalVertexBuffer;
                for(int i = 0; i < nrMeshes; i++)
                {
                    int meshNameSize;
                    int transformNameSize;
                    int materialNameSize;

                    ifs.read((char*)&meshNameSize, sizeof(int));
                    char* meshName = new char[meshNameSize];
                    ifs.read((char*)meshName, sizeof(char) * meshNameSize);
                    std::string t_meshName(meshName);

                    ifs.read((char*)&transformNameSize, sizeof(int));
                    char* transformName = new char[transformNameSize]; // denna meshens transforms, denna mesh ska använda dess transform värden!
                    ifs.read((char*)transformName, sizeof(char) * transformNameSize);

                    std::string t_transformName(transformName);

                    ifs.read((char*)&materialNameSize, sizeof(int));
                    char* materialName = new char[materialNameSize];
                    ifs.read((char*)materialName, sizeof(char) * materialNameSize);


                    int meshID;
                    ifs.read((char*)&meshID, sizeof(int)); // nog inget som används atm
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

                    XMFLOAT4X4 t_meshTransform = t_transformMap[t_transformName];


                    int jointID;
                    ifs.read((char*)&jointID, sizeof(int)); // här kommer jointID som ska vara för alla denna meshens vertiser, kom på att jag inte
                    // bygger vertiserna på denna sidan.
                    // Så du får sätta alla vertiser till detta värde när du bygger dem!!

                    // Nu måte verticerna från "Meshspace" till "joint space" som jag kallar det. Basicly byt koordinatsystem från meshens till
                    // jointens. Jointens origo mitten av jointen
                    // Osäker på om det är invers eller vanlig multi. Jag tror det är vanlig multi
                    // vi måste även lägga till jointID i vår vertis.
                    // Player specific
                    DoremiEngine::Graphic::MeshManager& meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
                    std::string textureName = m_materials[m_meshCoupling[0].materialName]; // Danger. Kan kraska om vi inte lagt in ett material
                    // innan... Finns inte stöd för material än TODOXX

                    // Compute vertexdata for graphics, discard data for physics.
                    DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f}; // TODOXX Should the scale for the player always be one?
                    std::vector<DirectX::XMFLOAT3> positionPX;
                    std::vector<int> indexPX;
                    vector<DoremiEngine::Graphic::Vertex>& vertexBuffer = ComputeVertexAndPositionAndIndex(meshData, scale, positionPX, indexPX);
                    size_t length = vertexBuffer.size();
                    // Omvandla vertexbuffern till skeletalvertexbuffern
                    for(size_t j = 0; j < length; j++)
                    {
                        XMVECTOR t_vertexPosition = XMLoadFloat3(&vertexBuffer[j].position);
                        XMMATRIX t_translationMatrix = XMLoadFloat4x4(&t_meshTransform);
                        XMVECTOR t_determinant = XMMatrixDeterminant(t_translationMatrix);
                        // t_vertexPosition = XMVector3Transform(t_vertexPosition, XMMatrixInverse(&t_determinant, t_translationMatrix));
                        // t_translationMatrix = XMMatrixInverse(&t_determinant, t_translationMatrix);
                        // t_vertexPosition = XMVector3Transform(t_vertexPosition, XMMatrixTranspose(t_translationMatrix));
                        t_vertexPosition = XMVector3Transform(t_vertexPosition, t_translationMatrix);
                        DoremiEngine::Graphic::SkeletalVertex t_vertex;
                        XMStoreFloat3(&t_vertex.position, t_vertexPosition);
                        t_vertex.normal = vertexBuffer[j].normal;
                        t_vertex.textureCoordinate = vertexBuffer[j].textureCoordinate;
                        t_vertex.jointID = jointID;
                        t_skeletalVertexBuffer.push_back(t_vertex);
                    }
                    delete transformName;
                    delete meshName;
                    delete materialName;
                }

                DoremiEngine::Graphic::MeshManager& meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
                meshManager.BuildSkeletalMeshInfoFromBuffer(t_skeletalVertexBuffer, t_sceneName);
                meshManager.BuildMaterialInfo(m_materials[m_meshCoupling[0].materialName]);
                CharacterDataNames o_charData;
                o_charData.meshName = t_sceneName;
                o_charData.materialName = m_materials[m_meshCoupling[0].materialName];
                ifs.close();

                p_skeletalInformation.Set(t_jointHeirarchy, t_animations);
                return o_charData;
            }
        }

        CharacterDataNames LevelLoaderClient::LoadCharacter(const std::string& p_fileName)
        {
            using namespace std;
            const string fileName = m_sharedContext.GetWorkingDirectory() + p_fileName;
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

                // LoadMaterialCharacter(ifs, nrMats);
                LoadMaterial(ifs, nrMats); // TODOKO THis should be instead of the ugly shit above
                // LoadTransformsCharacter(ifs, nrTransforms);
                LoadTransforms(ifs, nrTransforms); // TODOKO DOnt know... This or other shit when character is done...
                LoadMeshes(ifs, nrMeshes);
                LoadLights(ifs, nrLights);
                // BuildEntities();

                // Player specific
                DoremiEngine::Graphic::MeshManager& meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
                const std::string meshName = m_meshCoupling[0].meshName;
                std::string textureName;
                textureName = m_materials[m_meshCoupling[0].materialName].diffuseTextureName;

                // Compute vertexdata for graphics, discard data for physics.
                DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f}; // TODOXX Should the scale for the player always be one?
                std::vector<DirectX::XMFLOAT3> positionPX;
                std::vector<int> indexPX;
                vector<DoremiEngine::Graphic::Vertex>& vertexBuffer = ComputeVertexAndPositionAndIndex(m_meshes[meshName], scale, positionPX, indexPX);

                meshManager.BuildMeshInfoFromBuffer(vertexBuffer, meshName);
                meshManager.BuildMaterialInfo(m_materials[m_meshCoupling[0].materialName]);

                CharacterDataNames o_charData;
                o_charData.meshName = meshName;
                o_charData.materialName = textureName;
                return o_charData;
            }
            else
            {
                throw std::runtime_error("Could not load character.");
            }
        }

        void LevelLoaderClient::LoadFileInternal(const std::string& p_fileName) {}


        bool LevelLoaderClient::BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer)
        {
            bool r_shouldBuildPhysics = true;

            const ObjectCouplingInfo& meshCoupling = m_meshCoupling[p_meshCouplingID];

            EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::Transform);
            TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityId);

            transComp->position = m_transforms[meshCoupling.transformName].translation;
            transComp->rotation = m_transforms[meshCoupling.transformName].rotation;
            transComp->scale = m_transforms[meshCoupling.transformName].scale;

            DoremiEditor::Core::TransformData transformationData = m_transforms[meshCoupling.transformName];

            // If render, create grapic properties
            if(transformationData.attributes.isRendered)
            {
                EntityHandler::GetInstance().AddComponent(p_entityId, (int)ComponentType::Render);

                RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(p_entityId);

                DoremiEngine::Graphic::MeshManager& meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
                renderComp->mesh = meshManager.BuildMeshInfoFromBuffer(p_vertexBuffer, meshCoupling.meshName);

                std::string textureName = m_materials[meshCoupling.materialName].diffuseTextureName;

                renderComp->material = meshManager.BuildMaterialInfo(m_materials[meshCoupling.materialName]);
            }

            // if frequency platform
            if(transformationData.attributes.frequencyAffected)
            {
                r_shouldBuildPhysics = false;
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::NetworkObject) |
                                                                          static_cast<uint32_t>(ComponentType::RigidBody) |
                                                                          static_cast<uint32_t>(ComponentType::PhysicalMaterial));


                // Rigid body comp
                RigidBodyComponent* t_rigidBodyComp = GetComponent<RigidBodyComponent>(p_entityId);

                // t_rigidBodyComp->boxDims = dimension;
                t_rigidBodyComp->flags =
                    static_cast<RigidBodyFlags>(static_cast<uint32_t>(RigidBodyFlags::kinematic) | static_cast<uint32_t>(RigidBodyFlags::drain));
                t_rigidBodyComp->geometry = RigidBodyGeometry::dynamicBox;
            }

            // If non physic object
            if(transformationData.attributes.isSpawner || transformationData.attributes.spawnPointID > -1 ||
               transformationData.attributes.startOrEndPoint == 2 || transformationData.attributes.checkPointID > -1 || transformationData.attributes.isDangerous)
            {
                r_shouldBuildPhysics = false;
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


            return r_shouldBuildPhysics;
        }

        void LevelLoaderClient::BuildLights()
        {
            DoremiEngine::Graphic::LightManager& lightManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetLightManager();
            int i = 0;
            for(auto& light : m_lights)
            {
                lightManager.AddLight(light.type, light.intensity, light.colorDiffuse, light.coneAngle, light.direction, light.penumAgle,
                                      m_transforms[m_lightNames[i].first].translation);
                ++i;
            }
        }
    }
}
