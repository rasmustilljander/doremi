#include <Doremi/Core/Include/Handler/TreeHandler.hpp>

#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        TreeHandler* TreeHandler::m_singleton = nullptr;

        TreeHandler::TreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            m_treeCreator = new TreeCreator(p_sharedContext);
        }

        TreeHandler* TreeHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::cout << "Called GetInstance before TreeHandler was called" << std::endl;
            }
            return m_singleton;
        }
        void TreeHandler::StartupTreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartupTreeHandler was called");
            }
            m_singleton = new TreeHandler(p_sharedContext);
        }
        void TreeHandler::BuildTheTree() { m_treeCreator->CreateTree(); }
        TreeHandler::~TreeHandler() {}

        void TreeHandler::Update() {}
        void TreeHandler::OnEvent(Event* p_event) {}
    }
}