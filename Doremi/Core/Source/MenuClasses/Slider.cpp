// Project specific
#include <Doremi/Core/Include/MenuClasses/Slider.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>


// Third party

// Standard
#include <iostream>
#include <algorithm>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;

        Slider::Slider(SliderEffect p_effect, DoremiEngine::Graphic::MaterialInfo* p_materialInfoBack, DoremiEngine::Graphic::SpriteInfo* p_spriteInfoBack,
                       DoremiEngine::Graphic::MaterialInfo* p_materialInfoCircle, DoremiEngine::Graphic::SpriteInfo* p_spriteInfoCircle)
            : m_slideEffect(p_effect),
              m_materialInfoBack(p_materialInfoBack),
              m_spriteInfoBack(p_spriteInfoBack),
              m_materialInfoCircle(p_materialInfoCircle),
              m_spriteInfoCircle(p_spriteInfoCircle),
              m_isActive(false)
        {
        }
        Slider::~Slider() {}

        bool Slider::CheckIfInside(float p_mousePosX, float p_mousePosY)
        {
            DoremiEngine::Graphic::SpriteData& t_data = m_spriteInfoBack->GetData();

            // Check if to the right of the button
            if((p_mousePosX > t_data.position.x + t_data.origo.x + t_data.halfsize.x) || (p_mousePosX < t_data.position.x + t_data.origo.x - t_data.halfsize.x) ||
               (p_mousePosY < t_data.position.y + t_data.origo.y - t_data.halfsize.y) || (p_mousePosY > t_data.position.y + t_data.origo.y + t_data.halfsize.y))
            {
                m_isActive = false;
                return false;
            }
            // Otherwise we are on top of this button
            else
            {
                m_isActive = true;
                return true;
            }
            return false;
        }

        void Slider::UpdateSlider(float percent)
        {
            m_percent = percent;
            DoremiEngine::Graphic::SpriteData& t_dataCircle = m_spriteInfoCircle->GetData();
            DoremiEngine::Graphic::SpriteData& t_dataBack = m_spriteInfoBack->GetData();
            t_dataCircle.position.x = t_dataBack.position.x - t_dataBack.halfsize.x + percent * t_dataBack.halfsize.x * 2;
        }

        void Slider::UpdateSliderByMousePos(float p_mousePosX)
        {
            DoremiEngine::Graphic::SpriteData& t_dataBack = m_spriteInfoBack->GetData();
            float percent = ((p_mousePosX - t_dataBack.position.x) / (t_dataBack.halfsize.x * 1.0f) + 1.0f) / 2.0f;
            percent = std::max(percent, 0.0f);
            percent = std::min(percent, 1.0f);

            UpdateSlider(percent);
        }
    }
}