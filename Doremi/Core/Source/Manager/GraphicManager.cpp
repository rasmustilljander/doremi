// Project specific
#include <Manager/GraphicManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

namespace Doremi
{
    namespace Core
    {

        GraphicManager::GraphicManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext) {}


        GraphicManager::~GraphicManager() {}

        void GraphicManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Render | (int)ComponentType::Transform;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(i);
                    TransformComponent* orientationComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                    DirectX::XMFLOAT4X4 transMat;
                    DirectX::XMMATRIX tempTransMat =
                        DirectX::XMMatrixTranslation(orientationComp->position.x, orientationComp->position.y, orientationComp->position.z);
                    // DirectX::XMMATRIX tempRotMat = DirectX::XMMatrixRotationQuaternion();
                }
            }
        }

        void GraphicManager::OnEvent(Event* p_event) {}
    }
}