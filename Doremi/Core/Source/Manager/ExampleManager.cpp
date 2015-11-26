// Project specific
#include <Manager/ExampleManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/ExampleEvent.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        ExampleManager::ExampleManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            :Manager(p_sharedContext)
        {
			EventHandler::GetInstance()->Subscribe(Events::Example, this);
        }

        ExampleManager::~ExampleManager()
        {

        }


        void ExampleManager::Update(double p_dt)
		{
			//Example on how to create and Broadcast a event
			ExampleEvent* myEvent = new ExampleEvent();
			myEvent->eventType = Events::Example;
			myEvent->myInt = 42;
			EventHandler::GetInstance()->BroadcastEvent(myEvent);

            // Loop through all entities
            size_t length = EntityHandler::GetInstance()->GetLastEntityIndex();
            for (size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if (EntityHandler::GetInstance()->HasComponents(i, (int)ComponentType::Example))
                {
                    // Get component
                    ExampleComponent* t_example = EntityHandler::GetInstance()->GetComponentFromStorage<ExampleComponent>(i);
                    
                    // Perform desired operation
                    t_example->posX++;

                    // Instruct engine
                    m_sharedContext.GetPhysicsModule().ExampleMethod(t_example->posX);
                }
            }
        }
		void ExampleManager::OnEvent(Event* p_event)
		{
			// Check to see what event was received and do something with it (Might be changed to callback functions instead)
			switch (p_event->eventType)
			{
			case Events::Example:
			{
				//Cast the event to the correct format
				ExampleEvent* t_event = (ExampleEvent*)p_event;
				int t_intFromEvent = t_event->myInt;
				break;
			}
			default:
				break;
			}
		}
    }
}