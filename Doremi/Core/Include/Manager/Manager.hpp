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

            // Big update loop for every type of manager
            virtual void Update(double p_dt) = 0;

        protected:
            // Engine-context form which specific interfaces can be accessed
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}