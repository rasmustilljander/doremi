#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
            Contains information on position and rotation TODOJB Evaluate if both position and rotation should be contatined in same component
            TransformComponent is the actual draw position and TransformComponentPrevious/TransformComponentNext are for
            interpolating on the client. First and Second buffers are for buffering states, single buffer might theroetically cause visual
            displeasing behaviour when we get the next snapshot in different intervals befor & after frameticks
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

        struct TransformComponentBufferFirst
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            TransformComponentBufferFirst()
                : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };

        struct TransformComponentBufferSecond
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            TransformComponentBufferSecond()
                : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };
    }
}