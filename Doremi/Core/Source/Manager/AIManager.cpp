// Project specific
#include <Manager/AIManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <PlayerHandler.hpp>
#include <Helper/ProximityChecker.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>

// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialField.hpp>

// Standard
#include <iostream>
#include <DirectXMath.h>

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
            vector<int> t_fieldActors;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    bool inProx = ProximityChecker::GetInstance().CheckProximityToEntity(i, playerID);
                    if(inProx)
                    {
                        // HealthComponent* myHealth = EntityHandler::GetInstance().GetComponentFromStorage<HealthComponent>(i);
                        // myHealth->currentHealth--;
                        // if(myHealth->currentHealth < 0)
                        //{
                        //    // TODO This should be placed in a better location, like remove entity
                        //    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::RigidBody))
                        //    {
                        //        RigidBodyComponent* rb = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(i);
                        //        m_sharedContext.GetPhysicsModule().GetRigidBodyManager().RemoveBody(rb->p_bodyID);
                        //    }
                        //    EntityHandler::GetInstance().RemoveEntity(i);
                        //}
                    }
                }
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField | (int)ComponentType::Transform))
                {
                    t_fieldActors.push_back(i);
                }
            }
            // TODOKO remove from this place
            mask |= (int)ComponentType::Movement;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    XMFLOAT3 forceDirection = XMFLOAT3(0, 0, 0);
                    size_t nrofFieldActors = t_fieldActors.size();
                    for(size_t j = 0; j < nrofFieldActors; j++)
                    {
                        if(i != t_fieldActors[j])
                        {
                            float power =
                                EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(t_fieldActors[j])->ChargedActor->GetCharge();
                            float area =
                                EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(t_fieldActors[j])->ChargedActor->GetRange();
                            XMVECTOR fieldPos =
                                XMLoadFloat3(&EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(t_fieldActors[j])->position);
                            XMVECTOR addedForce = XMLoadFloat3(&forceDirection);
                            XMVECTOR myPos = XMLoadFloat3(&EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i)->position);

                            XMVECTOR distance = fieldPos - myPos;
                            float dist = *XMVector3Length(distance).m128_f32;
                            if(dist < area)
                            {
                                float force = power / dist;
                                // force = XMMax(force, 0.0f);
                                distance *= force;

                                addedForce += distance;
                                XMStoreFloat3(&forceDirection, addedForce);
                            }
                        }
                    }
                    XMVECTOR normalDirection = XMLoadFloat3(&forceDirection);
                    normalDirection = XMVector3Normalize(normalDirection);
                    XMStoreFloat3(&forceDirection, normalDirection);
                    EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i)->direction = forceDirection;
                    EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i)->forwardAcceleration = 100;
                }
            }
        }
        void AIManager::OnEvent(Event* p_event) {}
    }
}