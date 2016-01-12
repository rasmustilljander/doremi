// Project specific
#include <Manager/PlayerManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Subscriber.hpp>
#include <EventHandler/Events/ExampleEvent.hpp>
#include <EventHandler/Events/PlayerCreationEvent.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>


// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        PlayerManager::PlayerManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "PlayerManager")
        {
            EventHandler::GetInstance()->Subscribe(EventType::Example, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlayerCreation, this);
        }

        PlayerManager::~PlayerManager() {}


        void PlayerManager::Update(double p_dt)
        {
            //// Example on how to create and Broadcast a event

            //// Loop through all entities
            //size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            //for(size_t i = 0; i < length; i++)
            //{
            //    // Check that the current entity has the relevant components
            //    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Example) | (int)ComponentType::Example2)
            //    {

            //    }
            //}

            ////TransformComponent* t_trans = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(m_playerComponentID);
            //m_playerHandler->UpdatePosition();
        }
        void PlayerManager::OnEvent(Event* p_event)
        {
            //// Check to see what event was received and do something with it (Might be changed to callback functions instead)
            //switch(p_event->eventType)
            //{
            //    case Events::Example:
            //    {
            //        // Cast the event to the correct format
            //        ExampleEvent* t_event = (ExampleEvent*)p_event;
            //        int t_intFromEvent = t_event->myInt;
            //        break;
            //    }
            //    case Events::PlayerCreation:
            //    {
            //        PlayerCreationEvent* t_event = (PlayerCreationEvent*)p_event;
            //        m_playerComponentID = t_event->playerEntityID;
            //        m_playerHandler = new PlayerHandler(m_sharedContext);
            //        m_playerHandler->Initialize(m_playerComponentID);
            //        break;
            //    }
            //    default:
            //        break;
            //}
        }
    }
}