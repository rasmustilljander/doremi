#pragma once
// Project specific
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class ScreenObject
        {
        public:
            ScreenObject(DoremiEngine::Graphic::MaterialInfo* p_materialInfo, DoremiEngine::Graphic::MeshInfo* p_meshName, const XMFLOAT2& p_position,
                         const XMFLOAT2& p_extent);
            ScreenObject();
            virtual ~ScreenObject();
            XMFLOAT2 m_position;
            XMFLOAT2 m_extent;
            DoremiEngine::Graphic::MaterialInfo* m_materialInfo;
            DoremiEngine::Graphic::MeshInfo* m_meshInfo;
            // Kanske en texID för när den är highlightad?
            // Checks if mouse is inside this button
            XMFLOAT4X4 m_transformMatrix;

        private:
        };
    }
}
