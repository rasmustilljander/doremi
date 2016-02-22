#pragma once
#include <Doremi/Core/Include/TreeCreator.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

#include <DirectXMath.h>
#include <vector>
#include <cstdint>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}
namespace Doremi
{
    namespace Core
    {
        class TreeHandler : public Subscriber
        {
        public:
            static TreeHandler* GetInstance();

            static void StartupTreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void BuildTheTree();

            void Update();

            void OnEvent(Event* p_event) override;

        private:
            TreeHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~TreeHandler();

            static TreeHandler* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
            TreeCreator* m_treeCreator;
        };
    }
}
