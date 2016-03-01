#pragma once
#include <DirectXMath.h>
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
        */

        struct RenderComponent
        {
            DoremiEngine::Graphic::MeshInfo* mesh;
            DoremiEngine::Graphic::MaterialInfo* material;
            float offsetY = 0; // Offset in Y to make characters stand on the ground
            bool lockedRotationX = false; // Locks the rotation around the x axis when drawing
            RenderComponent(DoremiEngine::Graphic::MeshInfo* p_meshID, DoremiEngine::Graphic::MaterialInfo* p_materialID)
                : mesh(p_meshID), material(p_materialID)
            {
            }
            RenderComponent() : mesh(nullptr), material(nullptr) {}
        };
    }
}