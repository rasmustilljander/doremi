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
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>
#include <Doremi/Core/Include/PotentialFieldGridCreator.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
// Events
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/SpecialEntityCreatedEvent.hpp>
#include <EventHandler/Events/PlayerCreationEvent.hpp>
// Force Equations
#include <AIHelper/ForceImpactFunctions.hpp>
// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialField.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialGroup.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>

// Standard
#include <iostream>
#include <DirectXMath.h>
#include <set>
#include <functional>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        AIPathManager::AIPathManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "AIPathManager")
        {
            EventHandler::GetInstance()->Subscribe(EventType::RangedEnemyCreated, this);
            EventHandler::GetInstance()->Subscribe(EventType::MeleeEnemyCreated, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlayerCreation, this);
        }

        AIPathManager::~AIPathManager() {}


        void AIPathManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();

            for(size_t i = 0; i < length; i++)
            {
                // Update actors position, should perhaps not be here...
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField | (int)ComponentType::Transform))
                { // This is so the player updates his position too...
                    PotentialFieldComponent* pfComp = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i);
                    XMFLOAT3 pos = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i)->position;
                    if(!pfComp->isField) // If not a field we assume it's a actor who needs updating
                    {
                        pfComp->ChargedActor->SetPosition(pos);
                    }
                    else
                    {
                        pfComp->Field->SetCenter(pos);
                    }
                }
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::AIAgent | (int)ComponentType::Transform |
                                                                     (int)ComponentType::Movement | (int)ComponentType::PotentialField))
                {
                    // Get the needed components
                    XMFLOAT3 desiredPos;
                    TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                    XMFLOAT3 unitPos = transComp->position;
                    // DoremiEngine::AI::PotentialGroup* group = EntityHandler::GetInstance().GetComponentFromStorage<AIGroupComponent>(i)->Group;
                    DoremiEngine::AI::PotentialFieldActor* currentActor =
                        EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                    DoremiEngine::AI::PotentialField* field = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->Field;
                    // TODOEA BORDE SPARA UNDAN O INTE KOLLA X O Y EFTER VARANN
                    if(field != nullptr)
                    {
                        if(currentActor->GetPrevGridPos().x == field->WhatGridPosAmIOn(currentActor->GetPosition()).x &&
                           currentActor->GetPrevGridPos().y == field->WhatGridPosAmIOn(currentActor->GetPosition()).y)
                        {
                            // Remove the first in the list om vi skulle använda oss av delta_T för att uppdatera trailen med hjälp av den om någon
                            // står still.
                            // if we are still standing on the same quad as the last update we do nothing
                            // TODOKO if we have been standing stil for 2 long something might be wrong, Force him to move!!!
                        }
                        else
                        {
                            XMINT2 newPrevPos = field->WhatGridPosAmIOn(currentActor->GetPosition());
                            if(newPrevPos.x > -1 && newPrevPos.y > -1)
                            {
                                currentActor->SetPrevGridPosition(newPrevPos);
                                currentActor->UpdatePhermoneTrail(currentActor->GetPrevGridPos());
                            }
                        }
                        bool inField;
                        desiredPos = field->GetAttractionPosition(unitPos, inField, currentActor, false);
                        if(!inField)
                        {
                            // We are not in the current field, this means we are in another field. Lets change!
                            DoremiEngine::AI::PotentialField* newField =
                                m_sharedContext.GetAIModule().GetPotentialFieldSubModule().FindBestPotentialField(currentActor->GetPosition());
                            if(newField != nullptr && newField != field)
                            {
                                field->RemoveActor(currentActor);
                                newField->AddActor(currentActor);
                                EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->Field = newField;
                            }
                        }
                    }
                    // Should we jump?
                    float distance = ProximityChecker::GetInstance().CalculateDistance(desiredPos, unitPos);
                    if(distance > 8)
                    {
                        PerformJump(i);
                    }
                    XMFLOAT3 desiredPos3D = XMFLOAT3(desiredPos.x, unitPos.y, desiredPos.z); // The fields impact

                    XMVECTOR desiredPosVec = XMLoadFloat3(&desiredPos3D);
                    XMVECTOR unitPosVec = XMLoadFloat3(&unitPos);
                    XMVECTOR dirVec = desiredPosVec - unitPosVec;
                    dirVec = XMVector3Normalize(dirVec);

                    XMFLOAT3 direction;
                    XMStoreFloat3(&direction, dirVec * 0.5f); // TODOKO remove this hard coded shiat
                    MovementComponent* moveComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i);
                    moveComp->movement = direction;

                    currentActor->SetUsePhermonetrail(true); // set it to active so it's active as standard
                    currentActor->SetActivePotentialVsType(DoremiEngine::AI::AIActorType::Player,
                                                           false); // We dont know if we see the enemy so dont use this field
                }
            }
        }

        void AIPathManager::OnEvent(Event* p_event)
        {
            switch(p_event->eventType)
            {
                case Doremi::Core::EventType::MeleeEnemyCreated:
                {
                    SpecialEntityCreatedEvent* realEvent = static_cast<SpecialEntityCreatedEvent*>(p_event);
                    std::vector<DoremiEngine::AI::PotentialChargeInformation> specialCharges;
                    SetEnemyPotentialFieldStuff(realEvent->entityID, specialCharges);
                    break;
                }
                case Doremi::Core::EventType::RangedEnemyCreated:
                {
                    // Cast the event
                    SpecialEntityCreatedEvent* realEvent = static_cast<SpecialEntityCreatedEvent*>(p_event);

                    // Save a function to use for PF equation
                    std::function<float(float, float, float)> chargeEquation = ForceEquations::Standard;

                    // Make the special PF charges to be used when checking impact from specific actors
                    std::vector<DoremiEngine::AI::PotentialChargeInformation> specialCharges;
                    DoremiEngine::AI::PotentialChargeInformation newSpecial =
                        DoremiEngine::AI::PotentialChargeInformation(-100, 15, true, true, DoremiEngine::AI::AIActorType::Player, chargeEquation);
                    specialCharges.push_back(newSpecial);
                    DoremiEngine::AI::PotentialChargeInformation newSpecial3 =
                        DoremiEngine::AI::PotentialChargeInformation(-100, 4, true, true, DoremiEngine::AI::AIActorType::MeleeEnemy, chargeEquation);
                    specialCharges.push_back(newSpecial3);
                    SetEnemyPotentialFieldStuff(realEvent->entityID, specialCharges);
                    break;
                }
                case Doremi::Core::EventType::PlayerCreation:
                {
                    PlayerCreationEvent* realEvent = static_cast<PlayerCreationEvent*>(p_event);
                    if(EntityHandler::GetInstance().HasComponents(realEvent->playerEntityID, (int)ComponentType::PotentialField)) // Make sure the
                    // entity contains the needed stuff
                    {
                        DoremiEngine::AI::PotentialFieldActor* actor =
                            EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(realEvent->playerEntityID)->ChargedActor;
                        m_sharedContext.GetAIModule().GetPotentialFieldSubModule().AddActorToEveryPotentialField(actor);
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

        void AIPathManager::SetEnemyPotentialFieldStuff(const size_t& p_entityID, const std::vector<DoremiEngine::AI::PotentialChargeInformation>& p_specialCharges)
        {
            if(EntityHandler::GetInstance().HasComponents(p_entityID,
                                                          (int)ComponentType::PotentialField)) // Make sure the entity contains the needed stuff
            {

                // Get the actor from the component
                DoremiEngine::AI::PotentialFieldActor* actor =
                    EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(p_entityID)->ChargedActor;

                // Add the special charges
                size_t length = p_specialCharges.size();
                for(size_t i = 0; i < length; i++)
                {
                    actor->AddPotentialVsOther(p_specialCharges[i]);
                }

                DoremiEngine::AI::PotentialField* field;
                // Find the best suited field
                field = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().FindBestPotentialField(actor->GetPosition());
                // set the best suited field
                EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(p_entityID)->Field = field;
                if(field != nullptr)
                {
                    // Add the actor to the field, if not nullptr
                    field->AddActor(actor);
                }
                else
                {
                    // TODOKO log that a enemy started outside a field
                    std::cout << "A enemy spawn outside the field..." << std::endl;
                }
            }
            else
            {
                // TODOKO Log error no PF comp
            }
        }

        void AIPathManager::PerformJump(const int32_t& p_entityID)
        {
            if(EntityHandler::GetInstance().HasComponents(p_entityID, (int)ComponentType::Jump | (int)ComponentType::Gravity))
            {
                if(!EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(p_entityID)->travelSpeed > 0)
                {
                    // check if we have the jump component

                    JumpComponent* jumpComp = EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(p_entityID);
                    if(!jumpComp->active)
                    {
                        jumpComp->StartJump();
                    }
                }
            }
        }
    }
}