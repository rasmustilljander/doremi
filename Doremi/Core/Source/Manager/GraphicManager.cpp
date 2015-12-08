// Project specific
#include <Manager/GraphicManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>


namespace Doremi
{
    namespace Core
    {

        GraphicManager::GraphicManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            :Manager(p_sharedContext)
        {
        }


        GraphicManager::~GraphicManager()
        {
        }

        void GraphicManager::Update(double p_dt)  
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Audio;
            for (size_t i = 0; i < length; i++)
            {
                if (EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(i);
                    //m_sharedContext.GetGraphicModule().Draw(renderComp->meshID, renderComp->materialID,);
                }
            }
        }

        void GraphicManager::OnEvent(Event* p_event) 
        {

        }
    }
}