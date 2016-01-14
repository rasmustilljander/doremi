// This class
#include <PlayerClasses/GunController.hpp>
/// Engine
// Core
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
/// Game
// Handlers
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>

// Third party
#include <DirectXMath.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        GunController::GunController() {}
        GunController::~GunController() {}

        void GunController::FireGun(int p_playerID, const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            XMFLOAT3 bulletPosition = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_playerID)->position;
            // Create the new entity
            int bulletID = EntityHandler::GetInstance().CreateEntity(Blueprints::BulletEntity, bulletPosition);
            // Set it to be a friendly bullet
            EntityHandler::GetInstance().GetComponentFromStorage<EntityTypeComponent>(p_playerID)->type = EntityType::FriendlyBullet;

            /// Create the rigid body
            // Get material ID
            int matId = EntityHandler::GetInstance().GetComponentFromStorage<PhysicsMaterialComponent>(bulletID)->p_materialID;
            // Orientation is the same as that of the player
            XMFLOAT4 bulletOrientation = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_playerID)->rotation;
            // Calculate position by offsetting it along the target vector of the player

            XMVECTOR bulletPosVec = XMLoadFloat3(&bulletPosition);
            XMVECTOR targetVec = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
            XMMATRIX rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&bulletOrientation));
            targetVec = XMVector3Transform(targetVec, rotMat);
            float offsetAlongTarget = 2.5;
            bulletPosVec += targetVec * offsetAlongTarget;
            XMStoreFloat3(&bulletPosition, bulletPosVec);
            // Set dimensions to something smart TODOJB Collisions should be loaded from something sensible by now...
            XMFLOAT3 bulletDims = XMFLOAT3(0.5, 0.5, 0.5);
            p_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(bulletID, bulletPosition, bulletOrientation, bulletDims, matId);

            // Set start speed to something sensible
            float fireSpeed = 10;
            XMFLOAT3 fireVelocity;
            targetVec *= fireSpeed;
            XMStoreFloat3(&fireVelocity, targetVec);
            p_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(bulletID, fireVelocity);
        }
    }
}