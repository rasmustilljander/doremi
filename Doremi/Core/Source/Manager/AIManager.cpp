// Project specific
#include <Manager/AIManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <PlayerHandler.hpp>
#include <Helper/ProximityChecker.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>

// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        AIManager::AIManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext) {}

        AIManager::~AIManager() {}


        void AIManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::AIAgent | (int)ComponentType::Transform | (int)ComponentType::Health;
            size_t playerID = PlayerHandler::GetInstance()->GetPlayerEntityID();
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    bool inProx = ProximityChecker::GetInstance().CheckProximityToEntity(i, playerID);
                    if(inProx)
                    {
                        HealthComponent* myHealth = EntityHandler::GetInstance().GetComponentFromStorage<HealthComponent>(i);
                        myHealth->currentHealth--;
                        if(myHealth->currentHealth < 0)
                        {
                            // TODO This should be placed in a better location, like remove entity
                            if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::RigidBody))
                            {
                                RigidBodyComponent* rb = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(i);
                                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().RemoveBody(rb->p_bodyID);
                            }
                            EntityHandler::GetInstance().RemoveEntity(i);
                        }
                    }
                }
            }
        }
        void AIManager::OnEvent(Event* p_event) {}
    }
}