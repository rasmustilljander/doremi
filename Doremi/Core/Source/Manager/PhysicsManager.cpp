// Project specific
#include <Manager/PhysicsManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
#include <EntityComponent/EntityInterface.hpp>
#include <EntityComponent/Components/PhysicsComponent.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        PhysicsManager::PhysicsManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            :Manager(p_sharedContext)
        {

        }

        PhysicsManager::~PhysicsManager()
        {

        }

        void PhysicsManager::Update(double p_dt) 
        {
            EntityInterface* t_entityInterface = EntityInterface::GetInstance();
            int t_numberOfEntities = t_entityInterface->GetLastEntityIndex();
            for (size_t i = 0; i < t_numberOfEntities; i++)
            {
                int t_mask = (int)ComponentType::Physics;
                if (t_entityInterface->HasComponents(i, t_mask))
                {
                    PhysicsComponent* t_physicsComponent = t_entityInterface->GetComponentFromStorage<PhysicsComponent>(i);
                    t_physicsComponent->posX++;
                    cout << t_physicsComponent->posX << "Has now become: "
                        << m_sharedContext.GetPhysicsModule().DoSomething(t_physicsComponent->posX)
                        << endl;


                    //m_sharedContext.GetPhysicsModule().DoSomething(2);
                    //m_sharedContext.GetPhysicsModule().DoSomething(2);
                    //m_sharedContext.GetPhysicsModule().DoSomething(2);
                    //cout << 
                }
            }
        }
    }
}