#pragma once
// Project specific
#include <Manager/Manager.hpp>

namespace Doremi
{
    namespace Core
    {
        class ExampleManager : public Manager
        {
        public:
            ExampleManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~ExampleManager();
            void Update(double p_dt) override;
        };
    }
}

