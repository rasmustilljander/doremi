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

            for(auto playerID = t_playerMap.begin(); playerID != t_playerMap.end(); playerID++)
            {
                 //Get the players current frequency
                float t_currentFrequency = playerID->second->m_frequencyBufferHandler->GetFrequencyForFrame() / 900.0f;
                t_currentFrequency = std::min(t_currentFrequency, 1.0f);
                std::cout << t_currentFrequency << endl;
                // Loop through all entities
                const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
                // Loop over all entities to perform various functions on enteties that have sound components
                for(size_t j = 0; j < length; j++)
                {
                    // Check if entity will be affected by the frequencyanalyser.
                    if(EntityHandler::GetInstance().HasComponents(j, (int)ComponentType::FrequencyAffected))
                    {
                        if(ProximityChecker::GetInstance().CheckProximityToEntity(playerID->second->m_playerEntityID, j))
                        {
                            // Update j transform component in some way using the frequency we got earlier
                            if (EntityHandler::GetInstance().HasComponents(j, (int)ComponentType::PlatFormPatrolComponent))
                            {
                                using namespace DirectX;
                                PlatformPatrolComponent* t_platformPatrolComp = EntityHandler::GetInstance().GetComponentFromStorage<PlatformPatrolComponent>(j);
                                RigidBodyComponent* t_rigidBodyComponent = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(j);
                                TransformComponent* t_transformComponent = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(j);
                                XMFLOAT3 t_desiredPosition;
                                // Get the vector between the two points
                                XMVECTOR t_direction = XMLoadFloat3(&t_platformPatrolComp->endPosition) - XMLoadFloat3(&t_platformPatrolComp->startPosition);
                                t_currentFrequency = 1;
                                // Normalize the Frequency to a value between 0 and 1. This is hardcoded atm TODOLH
                                

                                //////// STARTA HÄRRRR fixa frekvens från jawwwwarn

                                // Calculate the desiredposition if frequency is held steady: take the vector between start and end. multiply with frequencyvalue 0 to 1. And add the start position
                                XMStoreFloat3(&t_desiredPosition, t_currentFrequency * t_direction + XMLoadFloat3(&t_platformPatrolComp->startPosition));

                                // Get the distance between desired and actual position of the platform
                                XMFLOAT3 t_distanceBetweenDesiredAndActualPosition;
                                XMStoreFloat3(&t_distanceBetweenDesiredAndActualPosition, XMVector3Length(XMLoadFloat3(&t_desiredPosition) - XMLoadFloat3(&t_transformComponent->position)));

                                // Get the normalized Vector between the to maximumpoints Then we move along that vector using the distance between desired and actual position 0.25 is hardcoded TODOLH
                                XMFLOAT3 t_normalizedDirection;
                                XMFLOAT3 t_velocityVector;
                                XMStoreFloat3(&t_velocityVector, XMVector3Normalize(t_direction) * t_distanceBetweenDesiredAndActualPosition.x * 0.25);

                                // Set the velocity of the platform to the vector we just calculated
                                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidBodyComponent->p_bodyID, t_velocityVector);


                            }
                        }
                    }
                }
            }
        }
    }
}
