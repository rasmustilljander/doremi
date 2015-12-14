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
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            TransformComponent() : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1)) {} // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };
    }
}