/// Project
#include <Manager/DamageManager.hpp>
#include <PlayerHandlerServer.hpp>
// Components
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>

// Events
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/DamageTakenEvent.hpp>
#include <EventHandler/Events/TriggerEvent.hpp>
#include <EventHandler/Events/PlaySoundEvent.hpp>

#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>

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
        DamageManager::DamageManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "DamageManager")
        {
            EventHandler::GetInstance()->Subscribe(EventType::Trigger, this);
        }

        DamageManager::~DamageManager() {}
        void DamageManager::Update(double p_dt)
        {
            std::map<uint32_t, PlayerServer*> t_players = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetPlayerMap();
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
                // If bullets collide a bullet could be in both and should be removed
                if(EntityHandler::GetInstance().HasComponents(t_collisionPairs[i].secondID, (int)ComponentType::EntityType))
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
                float totalDamage = 0; // Used to accumulate the damage
                for(size_t i = 0; i < length; i++)
                {
                    if(pairs.second->m_playerEntityID == t_bulletPairs[i].y)
                    {
                        // do we have a health comp?
                        std::cout << "Hit vs player detected" << std::endl;
                        if(EntityHandler::GetInstance().HasComponents(pairs.second->m_playerEntityID, (int)ComponentType::Health))
                        {
                            totalDamage += 10; // TODOCONFIG should probably be gathered from component or something
                        }
                    }
                }
                if(totalDamage != 0) // If we didnt take any damage dont send event
                {
                    DamageTakenEvent* t_damageTakenEvent = new DamageTakenEvent(totalDamage, pairs.second->m_playerEntityID);

                    EventHandler::GetInstance()->BroadcastEvent(t_damageTakenEvent);
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
            std::map<int, float> damageMap; // A map to save the total damage a entity have taken
            for(auto pairs : t_players)
            {
                // Getting our particlesystem
                if(EntityHandler::GetInstance().HasComponents(pairs.second->m_playerEntityID, (int)ComponentType::PressureParticleSystem))
                {
                    // checking if the particles in the particle system hit any drains.
                    std::vector<int> t_drainsHit = m_sharedContext.GetPhysicsModule().GetFluidManager().GetDrainsHit(pairs.second->m_playerEntityID);
                    size_t particleVecLength = t_drainsHit.size();
                    for(size_t o = 0; o < particleVecLength; o++)
                    {
                        if(t_drainsHit[o] != -1)
                        {
                            // if the drains hit is an enemy
                            if(EntityHandler::GetInstance().HasComponents(t_drainsHit[o], (int)ComponentType::Health | (int)ComponentType::AIAgent |
                                                                                              (int)ComponentType::Transform | (int)ComponentType::CharacterController))
                            {
                                if(damageMap.count(t_drainsHit[o]) == 0)
                                {
                                    damageMap[t_drainsHit[o]] = 0;
                                }
                                damageMap[t_drainsHit[o]] += 2; // TODOCONFIG Player damage
                            }
                        }
                        else
                        {
                            // nothing
                        }
                    }
                }
            }
            // loop through every damaged entity and send event. This is done so we dont send multiple events per damaged object
            for(auto pairs : damageMap)
            {
                DamageTakenEvent* t_damageTakenEvent = new DamageTakenEvent(pairs.second, pairs.first);
                EventHandler::GetInstance()->BroadcastEvent(t_damageTakenEvent);
            }
        }
        void DamageManager::OnEvent(Event* p_event)
        {
            // TODO not sure if dmg to ground trigger event should be placed here in damageManager
            if(p_event->eventType == EventType::Trigger)
            {
                TriggerEvent* t_triggerEvent = static_cast<TriggerEvent*>(p_event);

                if(t_triggerEvent->triggerType == TriggerType::DeathTrigger)
                {
                    if(EntityHandler::GetInstance().HasComponents(t_triggerEvent->objectEntityID, static_cast<uint32_t>(ComponentType::Health)))
                    {
                        if(static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->IsPlayer(t_triggerEvent->objectEntityID))
                        {
                            PlayerSpawnerHandler::GetInstance()->RespawnPlayer(t_triggerEvent->objectEntityID);

                            // // Send soundevent for death.
                            // AudioComponent* audioComp =
                            // EntityHandler::GetInstance().GetComponentFromStorage<AudioComponent>(t_triggerEvent->objectEntityID);
                            // PlaySoundEvent* playSoundEvent =
                            //   new PlaySoundEvent(t_triggerEvent->objectEntityID, audioComp->m_enumToSoundID[(int32_t)AudioCompEnum::Death]);
                            // EventHandler::GetInstance()->BroadcastEvent(playSoundEvent);
                        }
                        else
                        {
                            EntityHandler::GetInstance().RemoveEntity(t_triggerEvent->objectEntityID);
                        }
                    }
                }
            }
        }
    }
}