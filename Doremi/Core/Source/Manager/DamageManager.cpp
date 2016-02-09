/// Project
#include <Manager/DamageManager.hpp>
#include <PlayerHandler.hpp>
// Components
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
// Events
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/DamageTakenEvent.hpp>
/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

// Third Party
#include <DirectXMath.h>
#include <iostream>
#include <set>
namespace Doremi
{
    namespace Core
    {
        DamageManager::DamageManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "DamageManager") {}
        DamageManager::~DamageManager() {}
        void DamageManager::Update(double p_dt)
        {
            std::map<uint32_t, Player*> t_players = PlayerHandler::GetInstance()->GetPlayerMap();
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            // Check if the player was hit by any enemy bullet
            std::vector<DoremiEngine::Physics::CollisionPair> t_collisionPairs = m_sharedContext.GetPhysicsModule().GetCollisionPairs();
            size_t length = t_collisionPairs.size();

            // Start with getting every pair with a enemy bullet and save it to a list with bullet first, other second
            std::vector<XMINT2> t_bulletPairs;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(t_collisionPairs[i].firstID, (int)ComponentType::EntityType))
                {
                    EntityTypeComponent* typeComp = EntityHandler::GetInstance().GetComponentFromStorage<EntityTypeComponent>(t_collisionPairs[i].firstID);
                    if(((int)typeComp->type & (int)EntityType::EnemyBullet) == (int)EntityType::EnemyBullet) // if first entity is bullet
                    {
                        t_bulletPairs.push_back(XMINT2(t_collisionPairs[i].firstID, t_collisionPairs[i].secondID));
                    }
                }
                else if(EntityHandler::GetInstance().HasComponents(t_collisionPairs[i].secondID, (int)ComponentType::EntityType))
                {
                    EntityTypeComponent* typeComp = EntityHandler::GetInstance().GetComponentFromStorage<EntityTypeComponent>(t_collisionPairs[i].secondID);
                    if(((int)typeComp->type & (int)EntityType::EnemyBullet) == (int)EntityType::EnemyBullet) // if second entity is bullet
                    {
                        t_bulletPairs.push_back(XMINT2(t_collisionPairs[i].secondID, t_collisionPairs[i].firstID));
                    }
                }
            }
            // for each player we check if a bullet did hit us
            length = t_bulletPairs.size();
            for(auto pairs : t_players)
            {
                for(size_t i = 0; i < length; i++)
                {
                    if(pairs.second->m_playerEntityID == t_bulletPairs[i].y)
                    {
                        // do we have a health comp?
                        std::cout << "Hit vs player detected" << std::endl;
                        if(EntityHandler::GetInstance().HasComponents(pairs.second->m_playerEntityID, (int)ComponentType::Health))
                        {
                            HealthComponent* hpComp = EntityHandler::GetInstance().GetComponentFromStorage<HealthComponent>(pairs.second->m_playerEntityID);
                            hpComp->currentHealth -= 10; // TODOKO change to getting from comp
                            if(hpComp->currentHealth <= 0)
                            {
                                // u ded TODOKO
                                std::cout << "U DED!!!" << std::endl;
                            }
                        }
                    }
                }
            }
            // We need this set to ensure that a bullet that hits multiple targets isnt removed twice
            std::set<int> removedBullets;
            for(size_t i = 0; i < length; i++)
            {
                // Remove all bullets that hit something, TODOKO review if this is what we want
                if(removedBullets.count(t_bulletPairs[i].x) == 0)
                {
                    EntityHandler::GetInstance().RemoveEntity(t_bulletPairs[i].x);
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().RemoveBody(t_bulletPairs[i].x);
                    removedBullets.insert(t_bulletPairs[i].x);
                }
                else
                {
                    // The bullet did hit multiple targets but have already been removed, do nothing.
                }
            }
            // Check if the player hit any enemies

            // Look through our entities for the enemies

            size_t entitiesLength = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < entitiesLength; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PressureParticleSystem))
                {
                    std::vector<int> t_drainsHit = m_sharedContext.GetPhysicsModule().GetFluidManager().GetDrainsHit(i);
                    size_t particleVecLength = t_drainsHit.size();

                    for(size_t o = 0; o < particleVecLength; o++)
                    {
                        if(t_drainsHit[o] != -1)
                        {
                            if(EntityHandler::GetInstance().HasComponents(t_drainsHit[o], (int)ComponentType::Health | (int)ComponentType::AIAgent |
                                                                                              (int)ComponentType::Transform | (int)ComponentType::CharacterController))
                            {
                                HealthComponent* drainHitHpComp = EntityHandler::GetInstance().GetComponentFromStorage<HealthComponent>(t_drainsHit[o]);
                                // TODOEA Make it related to the guns damage and not hard coded
                                // std::cout << drainHitHpComp->currentHealth << std::endl;
                                // TODOCONFIG
                                drainHitHpComp->currentHealth -= 200; // TODOKO change to getting from comp

                                if(drainHitHpComp->currentHealth <= 0)
                                {
                                    // m_sharedContext.GetPhysicsModule().GetCharacterControlManager().RemoveCharacterController(t_drainsHit[o]);
                                    EntityHandler::GetInstance().RemoveEntity(t_drainsHit[o]);
                                    // DEBUG
                                    // std::cout << "Enemy DED!!!" << std::endl;
                                    // TODOSOUND Deathsound.
                                }
                                else
                                {
                                    // DEBUG
                                    // std::cout << "Enemy Hit!!" << std::endl;
                                    // TODOSOUND DamageSound
                                }
                            }
                        }
                        else
                        {
                            // nothing
                        }
                    }
                }
            }
        }
    }
}