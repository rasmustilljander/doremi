#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
            Contains information on position and rotation TODOJB Evaluate if both position and rotation should be contatined in same component
            TransformComponent is the actual draw position and TransformComponentPrevious/TransformComponentNext are for
            interpolating on the client.
        */
        struct TransformComponent
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale; // TODOKO Let him be!! no just kiddin, REMOVE!
            TransformComponent() : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1)) {}
        };

        struct TransformComponentPrevious
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            TransformComponentPrevious()
                : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };

        struct TransformComponentNext
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            TransformComponentNext()
                : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };

        struct TransformComponentOld
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            TransformComponentOld() : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };
    }
}