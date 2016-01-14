#pragma once
#include <InputHandler.hpp>
#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        class InputHandlerServer : public InputHandler
        {
        public:
            /**
                TODOCM doc
            */
            InputHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            void SetOrientationFromInput(DirectX::XMFLOAT4 p_orienation);

            /**
                TODOCM doc
            */
            DirectX::XMFLOAT4 GetOrientationFromInput();

        private:
            DirectX::XMFLOAT4 m_orientationQuaternion;
        };
    }
}