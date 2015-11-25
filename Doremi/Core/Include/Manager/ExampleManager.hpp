#pragma once
// Project specific
#include <Manager/Manager.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class ExampleManager : public Manager
        {
        public:
            ExampleManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~ExampleManager();
            void Update(double p_dt) override;
        };
    }
}

