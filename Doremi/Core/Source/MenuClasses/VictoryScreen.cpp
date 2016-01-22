// Project specific
#include <Doremi/Core/Include/MenuClasses/VictoryScreen.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        VictoryScreen::VictoryScreen(DoremiEngine::Graphic::MeshInfo* p_meshInfo) : m_meshInfo(p_meshInfo)
        {
            
        }
        VictoryScreen::VictoryScreen() {}
        VictoryScreen::~VictoryScreen() {}
        std::vector<ScreenObject*> VictoryScreen::GetScreen()
        {
            return m_screenObjects;
        }
        void VictoryScreen::AddScreenObject(DoremiEngine::Graphic::MaterialInfo* p_materialInfo, const XMFLOAT2& p_position, const XMFLOAT2& p_extent)
        {
            ScreenObject* t_screenObject = new ScreenObject(p_materialInfo, m_meshInfo, p_position, p_extent);
            m_screenObjects.push_back(t_screenObject);
        }
        
    }
}