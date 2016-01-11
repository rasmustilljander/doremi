// Project specific
#include <Manager/AI/AIPathManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <PlayerHandler.hpp>
#include <Helper/ProximityChecker.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/AIGroupComponent.hpp>

// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialField.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialGroup.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
// Standard
#include <iostream>
#include <DirectXMath.h>
#include <set>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        AIPathManager::AIPathManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext)
        {
            // TODOKO do this in a better place, might not work to have here in the future
            m_field = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewField(100, 100, 100, 100, XMFLOAT2(0, 0));
        }

        AIPathManager::~AIPathManager() {}


        void AIPathManager::Update(double p_dt)
        {
            size_t playerID = PlayerHandler::GetInstance()->GetDefaultPlayerEntityID();
            if(playerID == -1)
            {
                return;
            }

            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::AIAgent | (int)ComponentType::Transform | (int)ComponentType::Health;

            // TODOKO Should be done in a better way...

            if(firstUpdate)
            {
                int enemyID = -1;
                firstUpdate = false;
                // creating a invisible "wall", for testing only
                DoremiEngine::AI::PotentialFieldActor* actorwall =
                    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(XMFLOAT3(0, 0, 25), -10, 5);
                m_sharedContext.GetAIModule().GetPotentialFieldSubModule().AttachActor(*m_field, actorwall);
                m_field->Update();
                for(size_t i = 0; i < length; i++)
                {
                    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField))
                    {
                        // adding actors to field
                        // DoremiEngine::AI::PotentialFieldActor* actor =
                        // EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                        // m_sharedContext.GetAIModule().GetPotentialFieldSubModule().AttachActor(*m_field, actor);
                    }
                    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::AIGroup | (int)ComponentType::PotentialField))
                    {
                        enemyID = i;
                        DoremiEngine::AI::PotentialFieldActor* actor = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                        DoremiEngine::AI::PotentialGroup* group = EntityHandler::GetInstance().GetComponentFromStorage<AIGroupComponent>(i)->Group;
                        group->AddActor(actor);
                    }
                }
                if(enemyID != -1)
                {
                    DoremiEngine::AI::PotentialFieldActor* actor =
                        EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(playerID)->ChargedActor;
                    DoremiEngine::AI::PotentialGroup* group = EntityHandler::GetInstance().GetComponentFromStorage<AIGroupComponent>(enemyID)->Group;
                    group->AddActor(actor);
                }
            }

            // TO HERE

            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField | (int)ComponentType::Transform))
                {
                    DoremiEngine::AI::PotentialFieldActor* actor = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                    XMFLOAT3 pos = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i)->position;
                    actor->SetPosition(pos);
                }
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::AIAgent | (int)ComponentType::Transform |
                                                                     (int)ComponentType::Movement | (int)ComponentType::AIGroup))
                {
                    XMFLOAT2 desiredPos;
                    XMFLOAT3 unitPos = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i)->position;
                    DoremiEngine::AI::PotentialGroup* group = EntityHandler::GetInstance().GetComponentFromStorage<AIGroupComponent>(i)->Group;
                    DoremiEngine::AI::PotentialFieldActor* currentActor =
                        EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField))
                    {
                        desiredPos = m_field->GetAttractionPosition(unitPos, currentActor, true);
                    }
                    else
                    {
                        desiredPos = m_field->GetAttractionPosition(unitPos, nullptr, true);
                    }
                    XMFLOAT3 desiredPos3D = XMFLOAT3(desiredPos.x, unitPos.y, desiredPos.y);
                    XMFLOAT3 groupImpact = group->GetForceDirection(unitPos, currentActor);
                    XMVECTOR groupImpactVec = XMLoadFloat3(&groupImpact);
                    XMVECTOR desiredPosVec = XMLoadFloat3(&desiredPos3D);
                    XMVECTOR unitPosVec = XMLoadFloat3(&unitPos);
                    XMVECTOR dirVec = desiredPosVec - unitPosVec;
                    dirVec = XMVector3Normalize(dirVec);
                    dirVec += groupImpactVec * 0.2f; // TODOKO remove this variable!! Its there to make the static field more influencial
                    dirVec = XMVector3Normalize(dirVec);
                    XMFLOAT3 direction;
                    XMStoreFloat3(&direction, dirVec * 0.02f); // TODOKO remove this hard coded shiat
                    MovementComponent* moveComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i);
                    moveComp->movement = direction;
                }
            }
        }
        void AIPathManager::OnEvent(Event* p_event) {}
    }
}