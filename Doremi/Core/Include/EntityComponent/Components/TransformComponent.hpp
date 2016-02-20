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
            TransformComponent(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot, DirectX::XMFLOAT3 p_scale)
                : position(p_pos), rotation(p_rot), scale(p_scale)
            {
            }
            TransformComponent(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot) : position(p_pos), rotation(p_rot), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            }
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
            TransformComponentPrevious(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot, DirectX::XMFLOAT3 p_scale)
                : position(p_pos), rotation(p_rot), scale(p_scale)
            {
            }
            TransformComponentPrevious(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot)
                : position(p_pos), rotation(p_rot), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            }
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
            TransformComponentNext(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot, DirectX::XMFLOAT3 p_scale)
                : position(p_pos), rotation(p_rot), scale(p_scale)
            {
            }
            TransformComponentNext(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot)
                : position(p_pos), rotation(p_rot), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            }
        };

        struct TransformComponentSnapshotNext
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            uint32_t framesToNextCounter;
            TransformComponentSnapshotNext()
                : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1)), framesToNextCounter(0)
            {

            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
            TransformComponentSnapshotNext(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot)
                : position(p_pos), rotation(p_rot), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            }
            TransformComponentSnapshotNext(const TransformComponentNext& p_copy)
            {
                position = p_copy.position;
                rotation = p_copy.rotation;
                scale = p_copy.scale;
                framesToNextCounter = 0;
            }
            void IncrementFrame() { framesToNextCounter++; }
        };

        struct TransformComponentSnapshotPrevious
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 rotation;
            DirectX::XMFLOAT3 scale;
            uint32_t framesToNext;
            TransformComponentSnapshotPrevious()
                : position(DirectX::XMFLOAT3(0, 0, 0)), rotation(DirectX::XMFLOAT4(0, 0, 0, 1)), scale(DirectX::XMFLOAT3(1, 1, 1)), framesToNext(0)
            {
            } // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
            TransformComponentSnapshotPrevious(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT4 p_rot)
                : position(p_pos), rotation(p_rot), scale(DirectX::XMFLOAT3(1, 1, 1))
            {
            }
            TransformComponentSnapshotPrevious(const TransformComponentNext& p_copy)
            {
                position = p_copy.position;
                rotation = p_copy.rotation;
                scale = p_copy.scale;
                framesToNext = 0;
            }
        };
    }
}