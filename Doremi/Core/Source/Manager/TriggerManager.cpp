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
#include <EventHandler/Events/TriggerManagerEventTest.hpp>


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
            EventHandler::GetInstance()->Subscribe(EventType::TriggerManagerEventTest, this);
        }

        TriggerManager::~TriggerManager() {}


        void TriggerManager::Update(double p_dt)
        {
            // Example on how to create and Broadcast a event
            TriggerManagerEventStruct* myEvent = new TriggerManagerEventStruct();
            myEvent->eventType = EventType::TriggerManagerEventTest;
            myEvent->myInt = 42;
            myEvent->duOxo = 50;

            EventHandler::GetInstance()->BroadcastEvent(myEvent);

            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Trigger) | (int)ComponentType::Transform | (int)ComponentType::RigidBody)
                {
                    //////////////////////////////////////
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyStatic()
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic();
                    //    XMFLOAT3(quadSize.x * 0.5f, 3, quadSize.y * 0.5f), materialID);
                    // m_sharedContext.GetPhysicsModule().Update(0.017); // is this needed?
                    // std::vector<DoremiEngine::Physics::CollisionPair> collisionPairs = m_sharedContext.GetPhysicsModule().GetCollisionPairs();
                    // size_t collisionListLength = collisionPairs.size();
                    // int hej = t_entityHandler.GetInstance().GetLastEntityIndex();
                    // for (size_t i = 0; i < collisionListLength; ++i)
                    //{
                    //    if (myID == collisionPairs[i].firstID || myID == collisionPairs[i].secondID)
                    //    {
                    //        int objectID;
                    //        if (myID == collisionPairs[i].firstID)
                    //        {
                    //            objectID = collisionPairs[i].secondID;
                    //        }
                    //        else
                    //        {
                    //            objectID = collisionPairs[i].firstID;
                    //        }
                    //        if (EntityHandler::GetInstance().HasComponents(objectID, (int)ComponentType::PotentialField))
                    //        {
                    //            DoremiEngine::AI::PotentialFieldActor* actor =
                    //                EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(objectID)->ChargedActor;
                    //            actor->AddOccupiedQuad(XMINT2(x, z));
                    //            op_field->AddActor(actor);
                    //            break; // If more than on collision occures, fuck it >D
                    //        }
                    //    }
                    //}

                    ///////////////////


                    // Get component
                    ExampleComponent* t_example = EntityHandler::GetInstance().GetComponentFromStorage<ExampleComponent>(i);
                    Example2Component* t_example2 = EntityHandler::GetInstance().GetComponentFromStorage<Example2Component>(i);

                    // Perform desired operation
                    t_example->posX++;

                    // Instruct engine
                    m_sharedContext.GetPhysicsModule().ExampleMethod(t_example->posX);
                    // Give instructions where we want engine to alter data
                    m_sharedContext.GetPhysicsModule().ExampleMethodAltersData(&t_example->posX, &t_example->posY);
                    // Give instructions to engine where we want complex data to be changed
                    m_sharedContext.GetPhysicsModule().ExampleMethodAltersData(&t_example2->complexStruct.floatData, &t_example2->complexStruct.floatData);

                    // Test run GraphicsModule
                }
            }
        }
        void TriggerManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {
                case EventType::TriggerManagerEventTest:
                {
                    // Cast the event to the correct format
                    TriggerManagerEventStruct* t_event = (TriggerManagerEventStruct*)p_event;
                    int t_intFromEvent = t_event->myInt;
                    std::cout << "Tjena din fjant!=D" << std::endl;
                    break;
                }
                default:
                    break;
            }
        }
    }
}