// Project specific
#include <Doremi/Core/Include/MenuClasses/Slider.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;


        Slider::Slider(DoremiEngine::Graphic::MaterialInfo* p_materialInfoBack, DoremiEngine::Graphic::SpriteInfo* p_spriteInfoBack,
                       DoremiEngine::Graphic::MaterialInfo* p_materialInfoCircle, DoremiEngine::Graphic::SpriteInfo* p_spriteInfoCircle)
            : m_materialInfoBack(p_materialInfoBack), m_spriteInfoBack(p_spriteInfoBack), m_materialInfoCircle(p_materialInfoCircle), m_spriteInfoCircle(p_spriteInfoCircle)
        {
        }
        Slider::~Slider() {}

        bool Slider::CheckIfInside(float p_mousePosX, float p_mousePosY) { return false; }

        void Slider::UpdateSlider(float percent)
        {
            DoremiEngine::Graphic::SpriteData& t_dataCircle = m_spriteInfoCircle->GetData();
            DoremiEngine::Graphic::SpriteData& t_dataBack = m_spriteInfoBack->GetData();
            t_dataCircle.position.x = t_dataBack.position.x - t_dataBack.halfsize.x + percent * t_dataBack.halfsize.x * 2;
        }
    }
}