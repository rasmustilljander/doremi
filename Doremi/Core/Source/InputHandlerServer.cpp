#include <InputHandlerServer.hpp>

namespace Doremi
{
    namespace Core
    {
        InputHandlerServer::InputHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext) : InputHandler(p_sharedContext), m_orientationQuaternion(DirectX::XMFLOAT4(0,0,0,0))
        {

        }

        void InputHandlerServer::SetOrientationFromInput(DirectX::XMFLOAT4 p_orienation)
        {
            m_orientationQuaternion = p_orienation;
        }
    }
}