#pragma once
// Project specific
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <Doremi/Core/Include/Helper/MenuStates.hpp>
namespace Doremi
{
    namespace Core
    {
        struct MousePos
        {
            int x;
            int y;
        };
        using namespace DirectX;
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class Button
        {
            public:
            Button(const XMFLOAT2& p_position, const XMFLOAT2& p_size, DoremiEngine::Graphic::MaterialInfo* p_materialInfo,
                   DoremiEngine::Graphic::MeshInfo* p_meshInfo, Doremi::MenuStates::MenuState p_menuState);
            Button();
            virtual ~Button();
            XMFLOAT2 m_position;
            XMFLOAT2 m_size;
            DoremiEngine::Graphic::MaterialInfo* m_materialInfo;
            DoremiEngine::Graphic::MeshInfo* m_meshInfo;
            // Kanske en texID för när den är highlightad?
            // Checks if mouse is inside this button
            bool CheckIfInside(int p_mousePosX, int p_mousePosY);
            MenuStates::MenuState m_menuState;

            private:
            MousePos m_mousePos;
        };
    }
}
