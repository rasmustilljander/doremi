#include <string>
#include <map>
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/VertexStruct.hpp>
#include <vector>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Graphic
    {
        class MeshInfo;
    }
}

namespace Doremi
{
    namespace Core
    {
        // Loads of structs
        struct MaterialData
        {
            MaterialData()
            {
                mapMasks = 0;
                diffuse = 0;
                color[0] = color[1] = color[2] = 0.5f;
                ambColor[0] = ambColor[1] = ambColor[2] = 0.0f;
                specColor[0] = specColor[1] = specColor[2] = 0.0f;
                specCosine = specEccentricity = specRollOff = 0;
            }
            int mapMasks;
            float diffuse;
            float color[3];
            float ambColor[3];
            float specColor[3];
            float specCosine;
            float specEccentricity;
            float specRollOff;
        };

        struct MeshData
        {
            int nrPos, nrNor, nrUV, nrI;
            int triangleCount;
            DirectX::XMFLOAT3* positions;
            DirectX::XMFLOAT3* normals;
            DirectX::XMFLOAT2* uvs;

            int* indexPositions;
            int* indexNormals;
            int* indexUVs;
            // int *indexTriangles;
            int* trianglesPerFace;
            // Material *material; //pekar på ett specifikt material i en vektor av material
            ~MeshData()
            {
                // TODOKO should maybe be here
                //delete(positions);
                //delete(normals);
                //delete(uvs);

                //delete(indexPositions);
                //delete(indexNormals);
                //delete(indexUVs);
                //delete(trianglesPerFace);
            }
        };

        struct CustomAttributes
        {
            bool isRendered;
            bool isBBox;
            bool isCollider;
            bool isInteractable;
            float interactIntervalX;
            float interactIntervalY;
            float interactIntervalZ;
            int typeSpawner;
            int typeCheckPoint;
            int typeStartEnd;
            bool isAIground;
        };

        struct TransformData
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT4 rot;
            DirectX::XMFLOAT3 scale;
            CustomAttributes attributes; //custom attributesen!
        };

        struct CameraData
        {
            int isOrtho;
            float target[3];
            float upVector[3];
            float rightVector[3];
            float hAngle; // hor-FOV
        };

        struct LightData
        {
            int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
            int decayType; // 0 = none, 1 = linear, 2 = quadratic (l/d**v)
            float intensity;
            DirectX::XMFLOAT3 colorDiffuse;
            DirectX::XMFLOAT3 direction;
            float dropOff;
            float coneAngle;
            float penumAgle;
        };

        struct ObjectCouplingInfo
        {
            std::string transformName;
            std::string meshName;
            std::string materialName;
            ObjectCouplingInfo(const std::string& p_transformName, const std::string& p_meshName, const std::string& p_materialName)
                : transformName(p_transformName), meshName(p_meshName), materialName(p_materialName)
            {
            }
            ObjectCouplingInfo() {}
        };

        class LevelLoaderServer
        {
        public:
            LevelLoaderServer(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~LevelLoaderServer();
            void LoadLevel(const std::string& p_fileName);
            void LoadCharacter(const std::string& p_fileName);

        private:
            // Help functions
            std::vector<DoremiEngine::Graphic::Vertex> BuildMesh(const MeshData& p_data);
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            std::map<std::string, TransformData> m_transforms;
            std::map<std::string, MeshData> m_meshes;
            std::vector<ObjectCouplingInfo> m_meshCoupling;

            // HAX STUFF for physics magic
            DirectX::XMFLOAT3 m_currentScale;
            DirectX::XMFLOAT3 m_currentPos;
            DirectX::XMFLOAT4 m_currentOrientation;
            int m_entityID;
        };
    }
}