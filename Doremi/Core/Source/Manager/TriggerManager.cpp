// Project specific
#include <Manager/TriggerManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/TriggerEvent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
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
            // TODOXX If a trigger has the same wall as another trigger it acts weird. It will(tried it once) trigger one triggertype on the way out
            // and one on the
            // way in
            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Trigger | (int)ComponentType::Transform | (int)ComponentType::RigidBody))
                {

                    // Get the collisionpairs from physicmodule
                    std::vector<DoremiEngine::Physics::CollisionPair> collisionTriggerPairs = m_sharedContext.GetPhysicsModule().GetTriggerPairs();
                    size_t collisionListLength = collisionTriggerPairs.size();
                    for(size_t k = 0; k < collisionListLength; ++k)
                    {
                        // The first id will always be the trigger.
                        if(i == collisionTriggerPairs[k].firstID)
                        {
                            // setting up an event to broadcast the triggertype from the component trigger.
                            TriggerComponent* triggComp = EntityHandler::GetInstance().GetComponentFromStorage<TriggerComponent>(i);
                            TriggerEvent* myEvent =
                                new TriggerEvent(triggComp->triggerType, collisionTriggerPairs[k].secondID, collisionTriggerPairs[k].firstID);
                            EventHandler::GetInstance()->BroadcastEvent(myEvent);
                        }
                        else
                        {
                            // Do nothing
                        }
                    }
                }
                else
                {
                    // Do nothing
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