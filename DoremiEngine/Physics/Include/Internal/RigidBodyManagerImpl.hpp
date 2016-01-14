#pragma once
// Interface class
#include <RigidBodyManager.hpp>

// Internal
#include <Internal/RigidBodyClasses/MeshCooker.hpp>

// Third party
#include <DirectXMath.h>
#include <PhysX/PxPhysicsAPI.h>

// Standard libraries
#include <unordered_map>

using namespace physx;
using namespace std;
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        class RigidBodyManagerImpl : public RigidBodyManager
        {
        public:
            RigidBodyManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~RigidBodyManagerImpl();

            int AddBoxBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID) override;
            int AddBoxBodyStatic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID) override;
            void AddMeshBodyStatic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, vector<XMFLOAT3>& p_vertexPositions, vector<int>& p_indices,
                                   int p_materialID) override;
            void AddMeshBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, vector<XMFLOAT3>& p_vertexPositions,
                                    vector<int>& p_indices, int p_materialID) override;


            void SetCallback(int p_bodyID, int p_filterGroup, int p_filterMask) override;


            void AddForceToBody(int p_bodyID, XMFLOAT3 p_force) override;
            void AddTorqueToBody(int p_bodyID, XMFLOAT3 p_torque) override;
            void SetBodyVelocity(int p_bodyID, XMFLOAT3 p_velocity) override;
            void SetBodyAngularVelocity(int p_bodyID, XMFLOAT3 p_angleVeloctiy) override;
            void SetBodyPosition(int p_bodyID, XMFLOAT3 p_position, XMFLOAT4 p_orientation) override;
            void SetLinearDampening(int p_bodyID, float p_dampening) override;

            XMFLOAT3 GetBodyPosition(int p_bodyID) override;
            XMFLOAT4 GetBodyOrientation(int p_bodyID) override;
            XMFLOAT3 GetBodyVelocity(int p_bodyID) override;
            XMFLOAT3 GetBodyAngularVelocity(int p_bodyID) override;
            float GetLinearDampening(int p_bodyID) override;

            bool IsSleeping(int p_bodyID) override;

            void RemoveBody(int p_bodyID) override;

            // Internal methods not used via interface (only used in module)
            unordered_map<PxRigidActor*, int>& GetIDsByBodies();

        private:
            InternalPhysicsUtils& m_utils;
            MeshCooker* m_meshCooker;

            // Separate lists for dynamic and static bodies
            // unordered_map<int, PxRigidDynamic*> m_bodies;
            // unordered_map<int, PxRigidStatic*> m_staticBodies;
            unordered_map<int, PxRigidActor*> m_bodies;
            unordered_map<PxRigidActor*, int> m_IDsByBodies;
        };
    }
}