#pragma once
// Project specific
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Texture/SpriteInfo.hpp>

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;

        class ScreenObject
        {
        public:
            ScreenObject(DoremiEngine::Graphic::MaterialInfo* p_materialInfo, DoremiEngine::Graphic::SpriteInfo* m_spriteInfo);
            ScreenObject();
            virtual ~ScreenObject();

            DoremiEngine::Graphic::MaterialInfo* m_materialInfo;


            //// Add "Mesh" info here, but its buffer
            DoremiEngine::Graphic::SpriteInfo* m_spriteInfo;

        };
    }
}
