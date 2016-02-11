/// Project
#include <Doremi/Core/Include/Manager/AI/AITargetManager.hpp>
#include <PlayerHandler.hpp>

// Components
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
// Helper
#include <Helper/ProximityChecker.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
// Third party
#include <DirectXMath.h>
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        AITargetManager::AITargetManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "AITargetManager") {}

        AITargetManager::~AITargetManager() {}

        void AITargetManager::Update(double p_dt)
        {
            static int debug = 0;
            static double totTime = 0;
            totTime += p_dt;
            if(totTime > 3.0f)
            {
                totTime -= 3.0f;
            }
            else
            {
                return;
            }
            using namespace DirectX;
            // gets all the players in the world, used to see if we can see anyone of them
            std::map<uint32_t, Player*> t_players = PlayerHandler::GetInstance()->GetPlayerMap();
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            for(size_t i = 0; i < length; i++)
            {
                if(t_entityHandler.HasComponents(i, (int)ComponentType::Range | (int)ComponentType::AIAgent | (int)ComponentType::Transform))
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
                            TransformComponent* playerTransform = t_entityHandler.GetComponentFromStorage<TransformComponent>(playerID);
                            TransformComponent* AITransform = t_entityHandler.GetComponentFromStorage<TransformComponent>(i);
                            RangeComponent* aiRange = t_entityHandler.GetComponentFromStorage<RangeComponent>(i);
                            // Get things in to vectors
                            XMVECTOR playerPos = XMLoadFloat3(&playerTransform->position);

                            XMVECTOR AIPos = XMLoadFloat3(&AITransform->position);
                            // Calculate direction
                            XMVECTOR direction = playerPos - AIPos; // Might be the wrong way
                            direction = XMVector3Normalize(direction);
                            XMFLOAT3 directionFloat;
                            XMStoreFloat3(&directionFloat, direction);
                            // Offset origin of ray so we dont hit ourself
                            XMVECTOR rayOrigin = AIPos + direction * 5.0f; // TODOCONFIG x.xf is offset from the units body, might need to increase if
                            // the bodies radius is larger than x.x
                            XMFLOAT3 rayOriginFloat;
                            XMStoreFloat3(&rayOriginFloat, rayOrigin);
                            // Send it to physx for raycast calculation
                            int bodyHit = m_sharedContext.GetPhysicsModule().GetRayCastManager().CastRay(rayOriginFloat, directionFloat, 30);

                            if(bodyHit == playerID)
                            {
                                // We acctually did hit the player!!!! Let's kill him!!!!!
                                int id = t_entityHandler.CreateEntity(Blueprints::BulletEntity, rayOriginFloat);
                                // Get the components that we need to change/need to change other stuff
                                // RigidBodyComponent* rbComp = t_entityHandler.GetComponentFromStorage<RigidBodyComponent>(id);
                                PhysicsMaterialComponent* matComp = t_entityHandler.GetComponentFromStorage<PhysicsMaterialComponent>(id);
                                EntityTypeComponent* typeComp = t_entityHandler.GetComponentFromStorage<EntityTypeComponent>(id);

                                // Add a force to the body TODOXX should not be hard coded the force amount
                                direction *= 1500.0f;
                                XMFLOAT3 force;
                                XMStoreFloat3(&force, direction);
                                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(id, force);
                                typeComp->type = EntityType::EnemyBullet;
                            }
                        }
                    }
                }
            }
        }
    }
}