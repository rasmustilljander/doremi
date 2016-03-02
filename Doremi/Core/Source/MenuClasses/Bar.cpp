// Project specific
#include <Doremi/Core/Include/MenuClasses/Bar.hpp>
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

        Bar::Bar(const ScreenObject& p_barBar, const ScreenObject& p_back, const ScreenObject& p_front, float p_positionX, float p_maxSizeX)
            : m_barBar(p_barBar), m_barBack(p_back), m_barFront(p_front), m_positionX(p_positionX), m_maxSize(p_maxSizeX)
        {
        }

        Bar::Bar() {}
        Bar::~Bar() {}

        void Bar::UpdateProgress(float percent)
        {
            DoremiEngine::Graphic::SpriteData& t_data = m_barBar.m_spriteInfo->GetData();
            t_data.txtSize.x = percent;
            t_data.halfsize.x = m_maxSize * percent;
            t_data.position.x = m_positionX - m_maxSize * (1.0f - percent);
        }
    }
}