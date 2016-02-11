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
            virtual bool MoveController(int p_id, XMFLOAT3 p_discplacement, float p_dt) = 0;

            /**
            DEPRECATED!! Still works though*/
            virtual void SetCallback(int p_bodyID, int p_filterGroup, int p_filterMask) = 0;

            /**
            Big and kinda difficult-to-understand method to specify filtering for a given rigid body:
            p_thisIdMask - defines the body by assigning flags. When a collision occurs, the filtering
            checks for matches between the objects masks, and this mask

            p_notifyTouchOthersMask - defines which bodies we want to be notified when touching. All
            such collision pairs will be put in <insert appropriate list>

            p_notifyLeaveOthersmask - defines whcih bodies we want to be notified when leaving after a
            touch has occured. All such collision pairs are put in <insert appropriate list>

            p_ignoreOthersMask - defines which objects we want the object to completely ignore. If a
            match is found, the collision will simply not occur (kinda like a trigger. Except only
            against specific objects)*/ // TODOJB don't forget to update list
            virtual void SetCallbackFiltering(int p_bodyID, int p_thisIdMask, int p_notifyTouchOthersMask, int p_notifyLeaveOthersMask, int p_ignoreOthersMask) = 0;
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