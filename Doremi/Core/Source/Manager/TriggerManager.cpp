// Project specific
#include <Manager/TriggerManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/TriggerEvent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/CharacterEffectComponent.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>


// Third party

// Standard
#include <iostream>
using namespace std;

namespace Doremi
{
    namespace Core
    {
        TriggerManager::TriggerManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "TriggerManager") {}

        TriggerManager::~TriggerManager() {}


        void TriggerManager::Update(double p_dt)
        {
            // Get the collisionpairs from physicmodule
            std::vector<DoremiEngine::Physics::CollisionPair> collisionTriggerPairs = m_sharedContext.GetPhysicsModule().GetTriggerPairs();
            size_t collisionListLength = collisionTriggerPairs.size();
            for(size_t k = 0; k < collisionListLength; ++k)
            {
                // There might be bodies that aren't entities...
                if(collisionTriggerPairs[k].firstID >= 0)
                {
                    if(EntityHandler::GetInstance().HasComponents(collisionTriggerPairs[k].firstID,
                                                                  (int)ComponentType::Trigger | (int)ComponentType::Transform | (int)ComponentType::RigidBody))
                    {
                        // setting up an event to broadcast the triggertype from the component trigger.
                        TriggerComponent* triggComp = EntityHandler::GetInstance().GetComponentFromStorage<TriggerComponent>(collisionTriggerPairs[k].firstID);
                        TriggerEvent* myEvent = new TriggerEvent(triggComp->triggerType, collisionTriggerPairs[k].secondID, collisionTriggerPairs[k].firstID);
                        EventHandler::GetInstance()->BroadcastEvent(myEvent);
                    }
                    else
                    {
                        // There was an entity for this trigger, but no component. This should NEVER happen
                    }
                }
                else
                {
                    // This is one of the special non-entity triggers (Ground effect bodies, for instance)
                    if(collisionTriggerPairs[k].firstID == -15 && // TODOJB shouldn't be hard-coded to 15
                       EntityHandler::GetInstance().HasComponents(collisionTriggerPairs[k].secondID,
                                                                  (int)ComponentType::CharacterController | (int)ComponentType::CharacterEffect))
                    {
                        cout << "should apply effect" << endl;
                        EntityHandler::GetInstance().GetComponentFromStorage<CharacterEffectComponent>(collisionTriggerPairs[k].secondID)->effect =
                            CharacterEffect::CantStop;
                    }
                }
            }
        }
        void TriggerManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {

                default:
                    break;
            }
        }
    }
}