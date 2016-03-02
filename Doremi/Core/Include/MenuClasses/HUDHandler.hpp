#pragma once

// Project specific
#include <DirectXMath.h>
#include <vector>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {

        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class HUDHandler
        {
        public:

            static HUDHandler* GetInstance();
            static void StartHUDHandler(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT2 p_resolution);

            void Update(double p_dt);


        private:
            HUDHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~HUDHandler();

            static HUDHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext

        };
    }
}
