#pragma once
#include <DirectXMath.h>
using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        enum class RigidBodyGeometry : int
        {
            dynamicSphere,
            dynamicBox,
            staticBox,
            staticSphere,
            mesh,
        };

        enum class RigidBodyFlags : int
        {
            none,
            kinematic,
            trigger,
            drain,
            ignoredDEBUG,
        };
        /**
        Contain the ID for the rigid body of the entity with this component*/
        struct RigidBodyComponent
        {
            // Used for boxes
            XMFLOAT3 boxDims;
            // Used for spheres
            float sphereRadius;
            RigidBodyGeometry geometry;
            RigidBodyFlags flags = RigidBodyFlags::none;
        };
    }
}