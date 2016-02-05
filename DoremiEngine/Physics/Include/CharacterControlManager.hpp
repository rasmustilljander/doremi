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
            and y is radius).
            WARNING!! Ensure that a material already exists for the ID.*/
            virtual int AddController(int p_id, int p_matID, XMFLOAT3 p_position, XMFLOAT2 p_dimensions) = 0;

            /**
            Moves the desired controller with the specified displacement*/
            virtual int MoveController(int p_id, XMFLOAT3 p_discplacement, float p_dt) = 0;

            /**
            TODOJB docs*/
            virtual void SetCallback(int p_bodyID, int p_filterGroup, int p_filterMask) = 0;

            /**
            Gets the position of the character controller*/
            virtual XMFLOAT3 GetPosition(int p_id) = 0;

            /**
                Set the position of the character controller, warning is teleport and doesn't check for collisions
            */
            virtual void SetPosition(int p_id, XMFLOAT3 p_position) = 0;

            /**
            Sets whether or not the controller is a drain, which means that it
            will remove particles upon contact*/
            virtual void SetDrain(int p_id, bool p_isDrain) = 0;
            /**
            Removes the character controller
            */
            virtual void RemoveCharacterController(int p_bodyID) = 0;
            /**
            Gets the orientation of the character controller, as a quaternion*/
            virtual XMFLOAT4 GetOrientation(int p_id) = 0;
        };
    }
}