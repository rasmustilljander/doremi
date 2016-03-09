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

        Slider::Slider(DoremiEngine::Graphic::MaterialInfo* p_materialInfo, DoremiEngine::Graphic::SpriteInfo* p_spriteInfo)
            : m_materialInfo(p_materialInfo), m_spriteInfo(p_spriteInfo)
        {
        }
        Slider::Slider() {}
        Slider::~Slider() {}
    }
}