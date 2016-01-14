/// Game side
#include <LevelLoader.hpp>
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
/// Engine side
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
// Timing
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>
// Potential Field
#include <Doremi/Core/Include/PotentialFieldGridCreator.hpp>
// Physics

/// Standard
#include <sstream>
#include <fstream>

namespace Doremi
{
    namespace Core
    {
        LevelLoader::LevelLoader(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
        LevelLoader::~LevelLoader() {}
        void LevelLoader::LoadLevel(const std::string& p_fileName)
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

                    m_transforms[transformName] = transformData;
                }
                // ladda meshes. TODOSH Fixa så samma mesh itne läses in flera gånger, alltså så att samma mesh data inte finns på 2 ställen på GPU
                for(int i = 0; i < nrMeshes; i++)
                {
                    int transformNameSize;
                    int meshNameSize;

                    ifs.read((char*)&transformNameSize, sizeof(int));
                    ifs.read((char*)&meshNameSize, sizeof(int));

                    char* transformName = new char[transformNameSize];
                    char* meshName = new char[meshNameSize];

                    ifs.read((char*)transformName, sizeof(char) * transformNameSize);
                    ifs.read((char*)meshName, sizeof(char) * meshNameSize);


                    int materialNameSize;
                    ifs.read((char*)&materialNameSize, sizeof(int));

                    char* materialName = new char[materialNameSize];
                    ifs.read((char*)materialName, sizeof(char) * materialNameSize);

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
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfoFromBuffer(BuildMesh(meshData), meshName);
                    m_meshCoupling.push_back(ObjectCouplingInfo(transformName, meshName, materialName));
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
            size_t length = m_meshCoupling.size();
            for(size_t i = 0; i < length; i++)
            {
                std::string transformName = m_meshCoupling[i].transformName;
                std::string meshName = m_meshCoupling[i].meshName;
                int entityID = EntityHandler::GetInstance().CreateEntity(Blueprints::EmptyEntity);
                EntityHandler::GetInstance().AddComponent(entityID, (int)ComponentType::Render | (int)ComponentType::Transform);
                TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(entityID);

                transComp->position = m_transforms[transformName].pos;
                transComp->rotation = m_transforms[transformName].rot;
                transComp->scale = m_transforms[transformName].scale;

                renderComp->mesh = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().GetMeshInfo(meshName);
                if(renderComp->mesh == nullptr)
                {
                    int a = 5;
                }
                renderComp->material = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("AngryFace.dds");
            }
        }
        std::vector<DoremiEngine::Graphic::Vertex> LevelLoader::BuildMesh(const MeshData& p_data)
        {
            using namespace std;
            vector<DoremiEngine::Graphic::Vertex> vertexBuffer;

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

                tempV.position = p_data.positions[p_data.indexPositions[i + 1]];
                tempV.position.z = tempV.position.z * -1.0f;
                tempV.normal = p_data.normals[p_data.indexNormals[i + 1]];
                tempV.normal.z = tempV.normal.z * -1.0f;
                tempV.textureCoordinate = p_data.uvs[p_data.indexUVs[i + 1]];
                tempV.textureCoordinate.y = tempV.textureCoordinate.y - 1.0f;

                vertexBuffer.push_back(tempV);

                tempV.position = p_data.positions[p_data.indexPositions[i]];
                tempV.position.z = tempV.position.z * -1.0f;
                tempV.normal = p_data.normals[p_data.indexNormals[i]];
                tempV.normal.z = tempV.normal.z * -1.0f;
                tempV.textureCoordinate = p_data.uvs[p_data.indexUVs[i]];
                tempV.textureCoordinate.y = tempV.textureCoordinate.y - 1.0f;

                vertexBuffer.push_back(tempV);
            }

            return vertexBuffer;
        }
    }
}
