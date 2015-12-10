#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on position and rotation TODOJB Evaluate if both position and rotation should be contatined in same component
        */
        struct TransformComponent
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation; // Dont know if there is a specific quaternion in direct x
            TransformComponent() : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 0)) {}
        };
    }
}