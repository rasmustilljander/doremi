#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace std;
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
            Dynamic boxes are basically bodies which can be tossed around the world.
            Very basic method and will probably be expanded several.*/
            virtual int AddBoxBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID) = 0;
            /**
            Adds a static box body to the world. Returns the ID for the body.
            Static boxes stay still and are never affected by other physical entities.
            They can still be moved around however, just not by any other physical object.*/
            virtual int AddBoxBodyStatic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID) = 0;

            /**
            Adds a static body with a complex mesh as collision structure*/
            virtual void AddMeshBodyStatic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, vector<XMFLOAT3>& p_vertexPositions,
                                           vector<int>& p_indices, int p_materialID) = 0;
            /**
            NOT IMPLEMENTED. DO NOT USE TODOJB implement*/
            virtual void AddMeshBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, vector<XMFLOAT3>& p_vertexPositions,
                                            vector<int>& p_indices, int p_materialID) = 0;

            /**
            Adds a dynamic sphere to the world. This method creates a standard material (0,0,0)*/
            virtual int AddSphereBodyDynamic(int p_id, XMFLOAT3 p_position, float p_radius) = 0;

            /**
            Adds a dynamic capsule to the world. This method creates a standard material (0,0,0)*/
            virtual int AddCapsuleBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, float p_height, float p_radius) = 0;

            /**
            This can be used to make a body into a trigger. Triggers are not affected
            by the simulation. They trigger when colliding with other bodies, and a
            collision pair is generated*/
            virtual void SetTrigger(int p_id, bool p_isTrigger) = 0;

            /**
            Sets the body to be a drain. This means that particles which collide with
            the body are removed. No callbacks are triggered at this moment however.
            TODOJB Update as necessary*/
            virtual void SetDrain(int p_id, bool p_isDrain) = 0;


            /**
            TODOJB figure out what to do, and how to describe it in a comment
            */
            virtual void SetCallback(int p_bodyID, int p_filterGroup, int p_filterMask) = 0;

            /**
            Makes the body ignored by lots of thingies. This is just an experimental
            method to see if we can use drains in a fancy fashion TODOJB Make general*/
            virtual void SetIgnoredDEBUG(int p_bodyID) = 0;

            /// Manipulators
            /**
            Adds a force to a body*/
            virtual void AddForceToBody(int p_bodyID, XMFLOAT3 p_force) = 0;
            /**
            Adds torque (angular force) to a body*/
            virtual void AddTorqueToBody(int p_bodyID, XMFLOAT3 p_torque) = 0;
            /**
            Sets the velocity of a body*/
            virtual void SetBodyVelocity(int p_bodyID, XMFLOAT3 p_velocity) = 0;
            /**
            Sets angular velocity of the body*/
            virtual void SetBodyAngularVelocity(int p_bodyID, XMFLOAT3 p_angleVeloctiy) = 0;
            /**
            Sets the position and orientation of a body
            WARNING! Can cause strange behaviour. Better to use force and/or velocity*/
            virtual void SetBodyPosition(int p_bodyID, XMFLOAT3 p_position, XMFLOAT4 p_orientation) = 0;
            /**
            Sets linear dampening of object. Not sure what this is though*/
            virtual void SetLinearDampening(int p_bodyID, float p_dampeningCoef) = 0;

            /**
                Makes the actor a kinematic actor
            */
            virtual void SetKinematicActor(int p_bodyID, bool p_kinematic) = 0;
            /**
                Sets the Kinematic actors position
            */
            virtual void SetWorldPositionKinematic(int p_bodyID, XMFLOAT3 p_position) = 0;
            /**
                Sets the Kinematic actors posing using the previous position and moves along the inputparameter
            */
            virtual void MoveKinematicActor(int p_bodyID, XMFLOAT3 p_moveVector) = 0;

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

            /**
            Gets angular velocity of the body*/
            virtual XMFLOAT3 GetBodyAngularVelocity(int p_body) = 0;
            /**
            Gets linear dampening*/
            virtual float GetLinearDampening(int p_bodyID) = 0;

            /**
            Checks if a body is sleeping*/
            virtual bool IsSleeping(int p_body) = 0;

            /**
            Returns a vector with the IDs of all dynamic actors that woke up during
            the  previous simulation step through some sort of movement or collision*/
            virtual std::vector<int>& GetRecentlyWokenObjects() = 0;

            /**
            Returns a vector with the IDs of all dynamic actors that fell asleep in
            the previous simulation step due to inactivity*/
            virtual std::vector<int>& GetRecentlySleepingObjects() = 0;

            /**
            Releases the actor
            */
            virtual void RemoveBody(int p_bodyID) = 0;
        };
    }
}