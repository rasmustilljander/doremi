// Project specific
#include <Manager/TriggerManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/TriggerEvent.hpp>
#include <EntityComponent/Components/TriggerComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

#include <Doremi/Core/Include/PlayerHandler.hpp>


// Third party

// Standard
#include <iostream>
using namespace std;

namespace Doremi
{
    namespace Core
    {
        TriggerManager::TriggerManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "TriggerManager")
        {
            // EventHandler::GetInstance()->Subscribe(EventType::Trigger, this); //TODOEA REMOVE
        }

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
                // collisionTriggerPairs[k].firstID
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Trigger | (int)ComponentType::Transform | (int)ComponentType::RigidBody))
                {
                    std::vector<DoremiEngine::Physics::CollisionPair> collisionTriggerPairs = m_sharedContext.GetPhysicsModule().GetTriggerPairs();
                    size_t collisionListLength = collisionTriggerPairs.size();
                    for(size_t k = 0; k < collisionListLength; ++k)
                    {
                        if(i == collisionTriggerPairs[k].firstID)
                        {

                            TriggerEventStruct* myEvent = new TriggerEventStruct();
                            TriggerComponent* triggComp = EntityHandler::GetInstance().GetComponentFromStorage<TriggerComponent>(i);
                            myEvent->triggerType = triggComp->triggerType;
                            // myEvent->entityID = collisionTriggerPairs[k].secondID;
                            myEvent->entityID = collisionTriggerPairs[k].secondID;
                            EventHandler::GetInstance()->BroadcastEvent(myEvent);
                        }
                        else
                        {
                            // DO nathiiing
                        }
                    }
                }
                else
                {
                    // do nuting
                }
            }
        }
        void TriggerManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {
                // case EventType::Trigger: // TODOEA REMOVE!
                //{
                //    // Cast the event to the correct format
                //    TriggerEventStruct* t_event = (TriggerEventStruct*)p_event;
                //    int triggerID = t_event->entityID;
                //    cout << triggerID << endl;
                //    if (t_event->triggerType == TriggerType::GoalTrigger)
                //    {
                //        std::cout << "ReachGoalTriggered av denna fjant: " << std::endl;
                //    }
                //    if (t_event->triggerType == TriggerType::NoTrigger)
                //    {
                //        std::cout << "#hejsan" <<endl;
                //    }
                //    break;
                //}
                default:
                    break;
            }
        }
    }
}