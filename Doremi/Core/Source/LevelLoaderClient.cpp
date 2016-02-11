/// Game side
#include <LevelLoaderClient.hpp>
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
/// Engine side
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/LightManager.hpp>
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
                EntityHandler::GetInstance().AddComponent(p_entityId, static_cast<uint32_t>(ComponentType::NetworkObject));
            }

            // If non physic object
            if(transformationData.attributes.isSpawner || transformationData.attributes.spawnPointID > -1 ||
               transformationData.attributes.startOrEndPoint == 2 || transformationData.attributes.checkPointID > -1 || transformationData.attributes.isDangerous)
            {
                r_shouldBuildPhysics = false;
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
