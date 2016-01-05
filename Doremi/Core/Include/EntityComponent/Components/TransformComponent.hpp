#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on position and rotation
        */
        struct TransformComponent
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale; // TODOKO Let him be!! no just kiddin, REMOVE!
            TransformComponent() : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1)) {}
        };
    }
}