#pragma once
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Project specific
namespace Doremi
{
    namespace Core
    {
        class Manager
        {
        public:
            Manager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~Manager();

            virtual void Update(double p_dt) = 0;

        protected:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}