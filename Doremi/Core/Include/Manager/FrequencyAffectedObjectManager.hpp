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
            explicit FrequencyAffectedObjectManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~FrequencyAffectedObjectManager();
            void Update(double p_dt) override;
            /**
                Updates the position of the platformPatroller towards it's starting position. Like gravity to elevators NOT USED ATM. MIGHT NEED
               TEAKING IF U WANNA USE
            */
            void UpdatePlatformPatrollerMoveTowardsStartPosistion(int p_entityID);
            /**
                Updates the platformpatroller using the frequency to move towards it's endposition.
            */
            void UpdatePlatformPatrollerFrequencyMovement(int p_entityID, float p_currentFrequency);

        private:
            float m_frequencyNormalizer; // TODOLH TODOCM kanske ska göra frequencynormalizer för voice range på clienten innan man skickar till
            // servern
        };
    }
}
