// Project specific
#include <Manager/FrequencyAffectedObjectManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PlatformPatrolComponent.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/Helper/ProximityChecker.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>

#include <DirectXMath.h>
// Third party

// Standard
#include <iostream>
#include <algorithm>
using namespace std;

namespace Doremi
{
    namespace Core
    {
        FrequencyAffectedObjectManager::FrequencyAffectedObjectManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "FrequencyAffectedObjectManager")
        {
            m_frequencyNormalizer = 1000.0f;
        }

        FrequencyAffectedObjectManager::~FrequencyAffectedObjectManager() {}


        void FrequencyAffectedObjectManager::Update(double p_dt)
        {
            // First get the playermap from player handler
            std::map<uint32_t, Player*> t_playerMap = PlayerHandler::GetInstance()->GetPlayerMap();

            /*for(auto playerID = t_playerMap.begin(); playerID != t_playerMap.end(); playerID++)*/

            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            // Loop over all entities to perform various functions on enteties that have sound components
            for(size_t j = 0; j < length; j++)
            {
                // Check if entity will be affected by the frequencyanalyser.
                if(EntityHandler::GetInstance().HasComponents(j, (int)ComponentType::FrequencyAffected))
                {
                    // Code for other frequency affected objects here

                    // Update the platformpatrollers. First by making it move towards start position.
                    if(EntityHandler::GetInstance().HasComponents(j, (int)ComponentType::PlatFormPatrolComponent))
                    {
                        float t_gatheredFrequency = 0;
                        for(auto& playerID : t_playerMap)
                        {
                            // Get the players current frequency
                            float t_currentFrequency = playerID.second->m_frequencyBufferHandler->GetFrequencyForFrame() / 1000.0f;
                            t_currentFrequency = std::min(t_currentFrequency, 1.0f);
                            if(ProximityChecker::GetInstance().CheckProximityToEntity(playerID.second->m_playerEntityID, j))
                            {
                                t_gatheredFrequency += t_currentFrequency;
                            }
                            else
                            {
                                // do nothing
                            }
                        }
                        int t_length = t_playerMap.size();
                        if(t_length > 0)
                        {
                            UpdatePlatformPatrollerFrequencyMovement(j, t_gatheredFrequency / t_length);
                        }
                    }
                    else
                    {
                        // Do nothing
                    }
                }
                else
                {
                    // do nothing
                }
            }
        }
        void FrequencyAffectedObjectManager::UpdatePlatformPatrollerMoveTowardsStartPosistion(int p_entityID)
        {
            using namespace DirectX;
            PlatformPatrolComponent* t_platformPatrolComp = EntityHandler::GetInstance().GetComponentFromStorage<PlatformPatrolComponent>(p_entityID);
            TransformComponent* t_transformComponent = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityID);

            // First apply the "want to move to starting position"
            XMFLOAT3 t_moveTowardsStartPosition;
            XMStoreFloat3(&t_moveTowardsStartPosition,
                          XMVector3Normalize(XMLoadFloat3(&t_platformPatrolComp->startPosition) - XMLoadFloat3(&t_transformComponent->position)));

            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().MoveKinematicActor(p_entityID, XMFLOAT3(t_moveTowardsStartPosition.x * 0.01,
                                                                                                             t_moveTowardsStartPosition.y * 0.01,
                                                                                                             t_moveTowardsStartPosition.z * 0.01));
        }

        void FrequencyAffectedObjectManager::UpdatePlatformPatrollerFrequencyMovement(int p_entityID, float p_currentFrequency)
        {
            using namespace DirectX;
            PlatformPatrolComponent* t_platformPatrolComp = EntityHandler::GetInstance().GetComponentFromStorage<PlatformPatrolComponent>(p_entityID);
            TransformComponent* t_transformComponent = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityID);
            XMFLOAT3 t_desiredPosition;

            // Get the vector between the two points
            XMVECTOR t_direction = XMLoadFloat3(&t_platformPatrolComp->endPosition) - XMLoadFloat3(&t_platformPatrolComp->startPosition);

            // Calculate the desiredposition if frequency is held steady: take the vector between start and end. multiply with frequencyvalue 0 to 1.
            // And add the start position
            // XMStoreFloat3(&t_desiredPosition, p_currentFrequency * t_direction + XMLoadFloat3(&t_platformPatrolComp->startPosition));


            if(p_currentFrequency > 0.05)
            {
                t_desiredPosition = t_platformPatrolComp->endPosition;
            }
            else
            {
                t_desiredPosition = t_platformPatrolComp->startPosition;
            }
            // Get the direction again. This is from desired position to actual position. So we know what way to move
            XMVECTOR t_newDirection = XMLoadFloat3(&t_desiredPosition) - XMLoadFloat3(&t_transformComponent->position);

            // Get the distance between desired and actual position of the platform
            XMFLOAT3 t_distanceBetweenDesiredAndActualPosition;
            XMStoreFloat3(&t_distanceBetweenDesiredAndActualPosition, XMVector3Length(t_newDirection));

            // Get the normalized Vector between the to maximumpoints Then we move along that vector using the distance between desired and actual
            // position Removed desired to actual. Now steady Movespeed
            XMFLOAT3 t_normalizedDirection;
            XMFLOAT3 t_velocityVector;
            // If statement so that it doesnt "miss" the start position and wobbles around it. Same with end position
            if(t_distanceBetweenDesiredAndActualPosition.x > 0.4)
            {
                // XMVECTOR t_newDirection = XMLoadFloat3(&t_desiredPosition) - XMLoadFloat3(&t_transformComponent->position);
                float t_velo = p_currentFrequency * 1.5;
                XMStoreFloat3(&t_velocityVector, (XMVector3Normalize(t_newDirection) /** t_distanceBetweenDesiredAndActualPosition.x */ * (0.3 + t_velo)));
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().MoveKinematicActor(p_entityID,
                                                                                            t_velocityVector); // TODOLH This removes the update
            }
            else
            {
                // Do nothing
            }

            // Set the velocity of the platform to the vector we just calculated
        }
    }
}
