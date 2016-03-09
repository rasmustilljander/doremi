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

        class Slider
        {
        public:
            Slider(DoremiEngine::Graphic::MaterialInfo* p_materialInfoBack, DoremiEngine::Graphic::SpriteInfo* p_spriteInfoBack,
                   DoremiEngine::Graphic::MaterialInfo* p_materialInfoCircle, DoremiEngine::Graphic::SpriteInfo* p_spriteInfoCircle);

            virtual ~Slider();

            bool CheckIfInside(float p_mousePosX, float p_mousePosY);

            void UpdateSlider(float percent);

        private:
            DoremiEngine::Graphic::MaterialInfo* m_materialInfoBack;

            DoremiEngine::Graphic::MaterialInfo* m_materialInfoCircle;

            //// Add "Mesh" info here, but its buffer
            DoremiEngine::Graphic::SpriteInfo* m_spriteInfoBack;

            //// Add "Mesh" info here, but its buffer
            DoremiEngine::Graphic::SpriteInfo* m_spriteInfoCircle;
        };
    }
}
