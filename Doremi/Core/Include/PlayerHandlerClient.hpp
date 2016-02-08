#pragma once
#include <Doremi/Core/Include/PlayerHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        class NetworkEventReceiver;

        /**
            TODOCM doc
        */
        struct PlayerClient : public Player
        {
            PlayerClient(EntityID p_EntityID, InputHandler* p_inputHandler, FrequencyBufferHandler* p_frequencyBufferHandler, NetworkEventReceiver* p_networkEventReceiver)
                : Player(p_EntityID, p_inputHandler, p_frequencyBufferHandler), m_networkEventReceiver(p_networkEventReceiver)
            {
            }
            ~PlayerClient() { delete m_networkEventReceiver; }


            /**
                TODOCM doc
            */
            NetworkEventReceiver* m_networkEventReceiver;
        };

        class PlayerHandlerClient : public PlayerHandler
        {
        public:
            PlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~PlayerHandlerClient();

            static void StartPlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void Update(double p_dt) override;

            void UpdatePlayerInputs() override;

            void CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler) override;

            NetworkEventReceiver* GetNetworkEventReceiverForPlayer(uint32_t p_playerID);

        private:
        };
    }
}