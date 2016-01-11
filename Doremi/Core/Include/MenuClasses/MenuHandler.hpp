#pragma once
// Project specific
#include <Manager/Manager.hpp>
#include <EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class MenuHandler
        {
            public:
            MenuHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~MenuHandler();
            void Update(double p_dt);
            void Initialize();

            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}
