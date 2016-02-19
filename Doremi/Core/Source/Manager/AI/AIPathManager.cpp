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
            // TODOKO do this in a better place, might not work to have here in the future
            // m_field =
            //    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewField(250, 230, 100, 100,
            //                                                                              XMFLOAT3(-60, 3.0f, -40)); // Fits for first platform
            // m_topField =
            //    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewField(350, 500, 50, 50,
            //        XMFLOAT3(-280, 150.0f, -85)); // Fits for top platform
            EventHandler::GetInstance()->Subscribe(EventType::RangedEnemyCreated, this);
            EventHandler::GetInstance()->Subscribe(EventType::MeleeEnemyCreated, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlayerCreation, this);
            //////////////////////// Fixa PotFält
        }

        AIPathManager::~AIPathManager() {}


        void AIPathManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();

            // TODOKO Test wall
            if(firstUpdate)
            {
                firstUpdate = false;
            }
            for(size_t i = 0; i < length; i++)
            {
                // Update actors position, should perhaps not be here...
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PotentialField | (int)ComponentType::Transform))
                { // This is so the player updates his position too...
                    DoremiEngine::AI::PotentialFieldActor* actor = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(i)->ChargedActor;
                    XMFLOAT3 pos = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i)->position;
                    actor->SetPosition(pos);
                }
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::AIAgent | (int)ComponentType::Transform | (int)ComponentType::Movement |
                                                                      (int)ComponentType::PotentialField))
                {
                    // Get the needed components
                    XMFLOAT2 desiredPos;
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

                        desiredPos = field->GetAttractionPosition(unitPos, currentActor, false);
                    }
                    XMFLOAT3 desiredPos3D = XMFLOAT3(desiredPos.x, unitPos.y, desiredPos.y); // The fields impact
                    // XMFLOAT3 groupImpact = group->GetForceDirection(unitPos, currentActor); // The groups impact
                    // XMVECTOR groupImpactVec = XMLoadFloat3(&groupImpact);
                    XMVECTOR desiredPosVec = XMLoadFloat3(&desiredPos3D);
                    XMVECTOR unitPosVec = XMLoadFloat3(&unitPos);
                    XMVECTOR dirVec = desiredPosVec - unitPosVec;
                    dirVec = XMVector3Normalize(dirVec);
                    // dirVec += groupImpactVec * 0.2f; // TODOKO remove this variable!! Its there to make the static field more influencial
                    //  dirVec = XMVector3Normalize(dirVec);
                    XMFLOAT3 direction;
                    XMStoreFloat3(&direction, dirVec * 0.2f); // TODOKO remove this hard coded shiat
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

                // If we are to use PF instead of group when calculating enemies this is unecessary
                // The enemy should have a group before it enters this function, otherwise create a new
                //DoremiEngine::AI::PotentialGroup* group = EntityHandler::GetInstance().GetComponentFromStorage<AIGroupComponent>(p_entityID)->Group;
                //if(group == nullptr)
                //{
                //    // TODOKO Log error and what happens
                //    group = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewPotentialGroup();
                //}
                //group->AddActor(actor);

                // If we are to use groups instead of PF when calculating enemies this is unecessary
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
    }
}