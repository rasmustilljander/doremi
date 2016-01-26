/// Project
#include <Doremi/Core/Include/Manager/AI/AITargetManager.hpp>
#include <PlayerHandler.hpp>
// Components
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
// Helper
#include <Helper/ProximityChecker.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
// Third party
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        AITargetManager::AITargetManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "AITargetManager") {}

        AITargetManager::~AITargetManager() {}

        void AITargetManager::Update(double p_dt)
        {
            using namespace DirectX;
            std::map<uint32_t, Player*> t_players = PlayerHandler::GetInstance()->GetPlayerMap();
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Range | (int)ComponentType::AIAgent | (int)ComponentType::Transform))
                {
                    // They have a range component and are AI agents, let's see if a player is in range!
                    // I use proximitychecker here because i'm guessing it's faster than raycast
                    for(auto pairs : t_players)
                    {
                        int playerID = pairs.second->m_playerEntityID;
                        bool t_inRange = ProximityChecker::GetInstance().CheckProximityToEntity(i, playerID);
                        if(t_inRange)
                        {
                            // We are in range!! Let's raycast in a appropirate direction!! so start with finding that direction!
                            if(!EntityHandler::GetInstance().HasComponents(playerID, (int)ComponentType::Transform)) // Just for saftey :D
                            {
                                // TODOKO Log error!!!!!!!
                                return;
                            }
                            // Fetch some components
                            TransformComponent* playerTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(playerID);
                            TransformComponent* AITransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                            RangeComponent* aiRange = EntityHandler::GetInstance().GetComponentFromStorage<RangeComponent>(i);
                            // Get things in to vectors
                            XMVECTOR playerPos = XMLoadFloat3(&playerTransform->position);
                            XMVECTOR AIPos = XMLoadFloat3(&AITransform->position);
                            // Calculate direction
                            XMVECTOR direction = playerPos - AIPos; // Might be the wrong way
                            direction = XMVector3Normalize(direction);
                            XMFLOAT3 directionFloat;
                            XMStoreFloat3(&directionFloat, direction);
                            // Offset origin of ray so we dont hit ourself
                            XMVECTOR rayOrigin = playerPos + direction * 1.0f; // TODOXX x.xf is offset from the units body, might need to increase if
                            // the bodies radius is larger than x.x
                            XMFLOAT3 rayOriginFloat;
                            XMStoreFloat3(&rayOriginFloat, rayOrigin);
                            // Send it to physx for raycast calculation
                            int bodyHit = m_sharedContext.GetPhysicsModule().GetRayCastManager().CastRay(rayOriginFloat, directionFloat, aiRange->range);
                            if(bodyHit == playerID)
                            {
                                // We acctually did hit the player!!!! Let's kill him!!!!!
                                int a = 5;
                            }
                        }
                    }
                }
            }
        }
    }
}