#pragma once
#include <string>
#include <map>
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/VertexStruct.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <vector>
#include <fstream>
#include <DoremiEditor/Core/Include/TransformData.hpp>
#include <DoremiEditor/Core/Include/LightData.hpp>
#include <DoremiEditor/Core/Include/MeshData.hpp>
#include <DoremiEditor/Core/Include/MaterialData.hpp>
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>

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

        struct CharacterDataNames
        {
            std::string meshName;
            std::string materialName;
            std::vector<DoremiEngine::Graphic::Vertex> vertices;
        };

        class LevelLoader
        {
        public:
            /**
                TODO comment
            */
            LevelLoader(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODO comment
            */

            virtual ~LevelLoader();

        protected:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            // Help functions

            std::map<std::string, DoremiEditor::Core::TransformData> m_transforms;

            std::map<std::string, DoremiEditor::Core::CharacterTransformData> m_transformsCharacter; // TODOKO REMOVE@!@!#@$!

            std::map<std::string, DoremiEditor::Core::MaterialMessage> m_materials;

            std::map<std::string, DoremiEditor::Core::MeshData> m_meshes;
            std::vector<DoremiEditor::Core::LightData> m_lights;

            std::vector<ObjectCouplingInfo> m_meshCoupling;
            std::map<int, std::pair<std::string, std::string>> m_lightNames; // <index, <transformName, lightName>>

            // HAX STUFF for physics magic
            DirectX::XMFLOAT3 m_currentPos;
            DirectX::XMFLOAT4 m_currentOrientation;

            void LoadMaterial(std::ifstream& ifs, int nrMats);
            void LoadMaterialCharacter(std::ifstream& ifs, int nrMats); // TODOKO SHOULD BE REMOVED LATER
            void LoadTransforms(std::ifstream& ifs, int nrTransforms);
            void LoadTransformsCharacter(std::ifstream& ifs, int nrTransforms); // TODOKO Remove and replase when cahracte have own format
            void LoadMeshes(std::ifstream& ifs, int nrMeshes);
            void LoadLights(std::ifstream& ifs, int nrLights);
            void BuildEntities();
            virtual bool BuildComponents(int p_entityId, int p_meshCouplingID, std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer) = 0;

            void LoadTriggers();
            std::vector<DoremiEngine::Graphic::Vertex>
            ComputeVertexAndPositionAndIndex(const DoremiEditor::Core::MeshData& p_data, const DirectX::XMFLOAT3& p_scale,
                                             std::vector<DirectX::XMFLOAT3>& o_positionPX, std::vector<int>& o_indexPX);
            void SetPhysicalAttributesOnMesh(int p_entityID, std::vector<DirectX::XMFLOAT3>& p_positionPX, std::vector<int>& p_indexPX);
            void CalculateAABBBoundingBox(const std::vector<DoremiEngine::Graphic::Vertex>& p_vertexBuffer, const DoremiEditor::Core::TransformData& p_transformationData,
                                          DirectX::XMFLOAT3& o_max, DirectX::XMFLOAT3& o_min, DirectX::XMFLOAT3& o_center);
        };
    }
}