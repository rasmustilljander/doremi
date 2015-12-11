#pragma once
#include <DirectXMath.h>
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        class RigidBodyManager
        {
            public:
            /**
            Adds a dynamic box body to the world. Returns the ID for the body.
            Very basic method and will probably be expanded several*/
            virtual int AddBoxBodyDynamic(XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID) = 0;

            /// Manipulators
            /**
            Adds a force to a body*/
            virtual void AddForceToBody(int p_bodyID, XMFLOAT3 p_force) = 0;

            /// Getters
            /**
            Gets the position of a body*/
            virtual XMFLOAT3 GetBodyPosition(int p_bodyID) = 0;
            /**
            Gets the orientation of the body*/
            virtual XMFLOAT4 GetBodyOrientation(int p_body) = 0;
            /**
            Gets the velocity vector of the body*/
            virtual XMFLOAT3 GetBodyVelocity(int p_body) = 0;
        };
    }
}