#pragma once
#include <DirectXMath.h>
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        class CharacterControlManager
        {
            public:
            /**
            Adds a character controller and maps it to the specified ID.
            Controller starts at the specified position. It has the shape
            of a capsure, where p_dimensions defines its size (x is height
            and y is radius).*/
            virtual int AddController(int p_id, XMFLOAT3 p_position, XMFLOAT2 p_dimensions) = 0;

            /**
            Moves the desired controller with the specified displacement*/
            virtual int MoveController(int p_id, XMFLOAT3 p_discplacement, float p_dt) = 0;

            /**
            Gets the position of the character controller*/
            virtual XMFLOAT3 GetPosition(int p_id) = 0;

            /**
            Gets the orientation of the character controller, as a quaternion*/
            virtual XMFLOAT4 GetOrientation(int p_id) = 0;
        };
    }
}