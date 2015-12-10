// Project specific
#include <Manager/ExampleManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>
#include <EntityComponent/Components/Example2Component.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Events/ExampleEvent.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>


// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        ExampleManager::ExampleManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext)
        {
            EventHandler::GetInstance()->Subscribe(Events::Example, this);
        }

        ExampleManager::~ExampleManager() {}


        void ExampleManager::Update(double p_dt)
        {
            // Example on how to create and Broadcast a event
            ExampleEvent* myEvent = new ExampleEvent();
            myEvent->eventType = Events::Example;
            myEvent->myInt = 42;
            EventHandler::GetInstance()->BroadcastEvent(myEvent);

            // Loop through all entities
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Example) | (int)ComponentType::Example2)
                {
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

                    m_sharedContext.GetInputModule().Update(); // TODOEA
                }
            }
            // Test run GraphicsModule
            // Should obviously not be here....
            // Creates a rotating translation matrix for putting out the unit box
            // static float rot = 0;
            // rot += 0.005f;
            // DirectX::XMFLOAT4X4 world;
            // DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0,
            // 0.5, 1)), rot) * DirectX::XMMatrixTranslation(0, 0, 4.0f)));

            // DoremiEngine::Graphic::MeshInfo* temp =
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMeshInfo("test"); //TODOKO Remove memory leak
            // DoremiEngine::Graphic::MaterialInfo* temp1 =
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("TEST");
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*temp, *temp1, world);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
        }
        void ExampleManager::OnEvent(Event* p_event)
        {
            // Check to see what event was received and do something with it (Might be changed to callback functions instead)
            switch(p_event->eventType)
            {
                case Events::Example:
                {
                    // Cast the event to the correct format
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