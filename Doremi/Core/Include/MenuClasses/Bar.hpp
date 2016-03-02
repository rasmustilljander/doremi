#pragma once
// Project specific
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Texture/SpriteInfo.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;

        class Bar
        {
        public:
            Bar(const ScreenObject& p_bar, const ScreenObject& p_back, const ScreenObject& p_front, float p_positionX, float p_maxSizeX);
            Bar();
            virtual ~Bar();

            void UpdateProgress(float percent);

            float m_maxSize;
            float m_positionX;

            ScreenObject m_barBar;
            ScreenObject m_barBack;
            ScreenObject m_barFront;
        };
    }
}
