#include <GameCore.hpp>
#include <EntityComponent/EntityInterface.hpp>
#include <EntityComponent/Components/PhysicsComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        GameCore::GameCore()
        {
        }
        GameCore::~GameCore()
        {
        }

        // Only for testing, should be removed!
        void GenerateWorld()
        {
            PhysicsComponent* t_physicsComponent = new PhysicsComponent(5, 5);
        }

        void GameCore::Initialize()
        {
            m_enitiyInterface = EntityInterface::GetInstance();
            GenerateWorld();
        }
        void GameCore::StartCore()
        {
            while(true)
            {
            }
        }
    }
}