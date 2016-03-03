#pragma once
// Project specific
#include <DirectXMath.h>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;
        class MaterialInfo;
    }
    namespace Core
    {
        class SharedContext;
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
            static void StartupVictoryScreen(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static VictoryScreen* GetInstance();

            auto GetScreenObjects() { return m_screenObjects; }

            void Update();

        private:
            static VictoryScreen* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            std::vector<ScreenObject*> m_screenObjects;
            DoremiEngine::Graphic::MeshInfo* m_meshInfo;
            ScreenObject m_background;

            VictoryScreen(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~VictoryScreen();
        };
    }
}
