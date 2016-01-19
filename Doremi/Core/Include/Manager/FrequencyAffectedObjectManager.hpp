#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        This Manager is supposed to get frequency information from the frequencybufferhandler and fetch the list of players to analyse which objects
        in the world should be affected by
        playerfrequencies and update those objects
        */


        class FrequencyAffectedObjectManager : public Manager
        {
        public:
            FrequencyAffectedObjectManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~FrequencyAffectedObjectManager();
            void Update(double p_dt) override;


        private:
        };
    }
}
