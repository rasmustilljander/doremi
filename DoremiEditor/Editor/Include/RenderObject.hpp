#pragma once
#include <DirectXMath.h>
using namespace DirectX;
namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;
        class MaterialInfo;
    }
}
namespace Doremi
{
    namespace Core
    {
        /**
        Should contain data on what mech and what material to use. DO NOT DELETE THESE POINTERS
        Striking similarity to RenderComponent combined with transform component
        */

        struct RenderObject
        {
            // Transformation stuff
            XMFLOAT3 position;
            XMFLOAT4 orientation;
            XMFLOAT3 scale;
            // Render stuff
            DoremiEngine::Graphic::MeshInfo* mesh;
            DoremiEngine::Graphic::MaterialInfo* material;
            float offsetY = 0; // Offset in Y to make characters stand on the ground
            RenderObject(DoremiEngine::Graphic::MeshInfo* p_meshID, DoremiEngine::Graphic::MaterialInfo* p_materialID)
                : mesh(p_meshID), material(p_materialID)
            {
            }
            RenderObject() : mesh(nullptr), material(nullptr) {}
        };
    }
}