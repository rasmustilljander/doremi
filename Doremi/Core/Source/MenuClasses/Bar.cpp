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

        Bar::Bar(const ScreenObject& p_back, const ScreenObject& p_front) : m_barBack(p_back), m_barFront(p_front) {}
        Bar::Bar() {}
        Bar::~Bar() {}

        void Bar::UpdateProgress(float percent) {}
    }
}