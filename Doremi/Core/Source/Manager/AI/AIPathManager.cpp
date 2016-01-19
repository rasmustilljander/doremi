/// Project specific
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
#include <Doremi/Core/Include/PotentialFieldGridCreator.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
// Events
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/EntityCreatedEvent.hpp>
#include <EventHandler/Events/PlayerCreationEvent.hpp>

// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
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
        AIPathManager::AIPathManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "AIPathManager")
        {
            // TODOKO do this in a better place, might not work to have here in the future
            m_field = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewField(100, 100, 100, 100, XMFLOAT3(0, 5, 0));
            EventHandler::GetInstance()->Subscribe(EventType::AiGroupActorCreation, this);
            EventHandler::GetInstance()->Subscribe(EventType::PotentialFieldActorCreation, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlayerCreation, this);
            //////////////////////// Fixa PotFält

            // Testar TODOEA
            Core::EntityHandler& t_entityHandler = Core::EntityHandler::GetInstance();

            /// debugskit
            PotentialFieldGridCreator t_potentialFieldGridCreator = PotentialFieldGridCreator(m_sharedContext);
            t_potentialFieldGridCreator.BuildGridUsingPhysicXAndGrid(m_field);
            // creating a invisible "wall", for testing only
            m_field->Update();

            //&& render pos rigidbody, potentialfield
        }

        AIPathManager::~AIPathManager() {}


        void AIPathManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::AIAgent | (int)ComponentType::Transform | (int)ComponentType::Health;

            // TODOKO Test wall
            //if(firstUpdate)
            //{
            //    firstUpdate = false;

            //}

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
                    DoremiEngine::AI::PotentialFieldActor* currentActor = nullptr;
                    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField))
                    {
                        currentActor = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                        desiredPos = m_field->GetAttractionPosition(unitPos, currentActor, false);
                    }
                    else
                    {
                        desiredPos = m_field->GetAttractionPosition(unitPos, nullptr, false);
                    }
                    XMFLOAT3 desiredPos3D = XMFLOAT3(desiredPos.x, unitPos.y, desiredPos.y); // The fields impact
                    XMFLOAT3 groupImpact = group->GetForceDirection(unitPos, currentActor); // The groups impact
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
        void AIPathManager::OnEvent(Event* p_event)
        {
            switch(p_event->eventType)
            {
                case Doremi::Core::EventType::PotentialFieldActorCreation:
                {
                    EntityCreatedEvent* realEvent = static_cast<EntityCreatedEvent*>(p_event);
                    if(EntityHandler::GetInstance().HasComponents(realEvent->entityID, (int)ComponentType::PotentialField)) // Make sure the entity
                    // contains the needed
                    // stuff
                    {
                        DoremiEngine::AI::PotentialFieldActor* actor =
                            EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(realEvent->entityID)->ChargedActor;
                        m_field->AddActor(actor);
                    }
                    else
                    {
                        // TODOKO Log error
                    }
                    break;
                }
                case Doremi::Core::EventType::AiGroupActorCreation:
                {
                    EntityCreatedEvent* realEvent = static_cast<EntityCreatedEvent*>(p_event);
                    if(EntityHandler::GetInstance().HasComponents(realEvent->entityID, (int)ComponentType::PotentialField | (int)ComponentType::AIGroup)) // Make sure the entity contains the needed stuff
                    {
                        DoremiEngine::AI::PotentialFieldActor* actor =
                            EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(realEvent->entityID)->ChargedActor;
                        DoremiEngine::AI::PotentialGroup* group =
                            EntityHandler::GetInstance().GetComponentFromStorage<AIGroupComponent>(realEvent->entityID)->Group;
                        if(group == nullptr)
                        {
                            // TODOKO Log error and what happens
                            group = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewPotentialGroup();
                        }
                        group->AddActor(actor);
                    }
                    else
                    {
                        // TODOKO Log error
                    }
                    break;
                }
                case Doremi::Core::EventType::PlayerCreation:
                {
                    PlayerCreationEvent* realEvent = static_cast<PlayerCreationEvent*>(p_event);
                    if(EntityHandler::GetInstance().HasComponents(realEvent->playerEntityID, (int)ComponentType::PotentialField)) // Make sure the
                    // entity contains
                    // the needed stuff
                    {
                        DoremiEngine::AI::PotentialFieldActor* actor =
                            EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(realEvent->playerEntityID)->ChargedActor;
                        m_field->AddActor(actor);
                    }
                    else
                    {
                        // TODOKO log error
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}