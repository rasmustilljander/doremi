#pragma once
// Project specific
#include <DirectXMath.h>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;
        class MaterialInfo;
    }
}

namespace Doremi
{
    namespace Core
    {
        class ScreenObject;
        using namespace DirectX;
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class VictoryScreen
        {
        public:
            VictoryScreen(DoremiEngine::Graphic::MeshInfo* p_meshInfo);
            VictoryScreen();
            virtual ~VictoryScreen();
            std::vector<ScreenObject*> GetScreen();
            void AddScreenObject(DoremiEngine::Graphic::MaterialInfo* p_materialInfo, const XMFLOAT2& p_position, const XMFLOAT2& p_extent);

        private:
            std::vector<ScreenObject*>m_screenObjects;
            DoremiEngine::Graphic::MeshInfo* m_meshInfo;
        };
    }
}
