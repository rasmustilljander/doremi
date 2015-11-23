// Project specific
#include <Manager/ExampleManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EntityComponent/EntityInterface.hpp>
#include <EntityComponent/Components/ExampleComponent.hpp>

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

        }

        ExampleManager::~ExampleManager()
        {

        }


        void ExampleManager::Update(double p_dt)
        {
            // Loop through all entities
            size_t length = EntityInterface::GetInstance()->GetLastEntityIndex();
            for (size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if (EntityInterface::GetInstance()->HasComponents(i, (int)ComponentType::Example))
                {
                    // Get component
                    ExampleComponent* t_example = EntityInterface::GetInstance()->GetComponentFromStorage<ExampleComponent>(i);
                    
                    // Perform desired operation
                    t_example->posX++;

                    // Instruct engine
                    m_sharedContext.GetPhysicsModule().ExampleMethod(t_example->posX);
                }
            }
        }
    }
}