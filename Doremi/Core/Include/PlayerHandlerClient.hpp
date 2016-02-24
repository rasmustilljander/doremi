#pragma once
#include <Doremi/Core/Include/PlayerHandler.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class Logger;
    }
}

namespace Doremi
{
    namespace Core
    {
        class NetworkEventReceiver;
        class NetworkStreamer;

        /**
            TODOCM doc
        */
        struct PlayerClient : public Player
        {
            PlayerClient() : Player(0, nullptr, nullptr) {}
            PlayerClient(EntityID p_EntityID, InputHandler* p_inputHandler, FrequencyBufferHandler* p_frequencyBufferHandler, NetworkEventReceiver* p_networkEventReceiver)
                : Player(p_EntityID, p_inputHandler, p_frequencyBufferHandler), m_networkEventReceiver(p_networkEventReceiver), IsCreated(false)
            {
            }
            ~PlayerClient() { delete m_networkEventReceiver; }


            /**
                TODOCM doc
            */
            NetworkEventReceiver* m_networkEventReceiver;

            bool IsCreated;
        };

        class PlayerHandlerClient : public PlayerHandler, public Subscriber
        {
        public:
            static void StartPlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            EntityID GetPlayerEntityID();

            void Update(double p_dt) override;

            void UpdatePlayerRotations(Player* p_player);

            void UpdatePlayerInputs();

            void SetNewPlayerEntityID(const EntityID& p_entityID);

            NetworkEventReceiver* GetNetworkEventReceiverForPlayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            void ReadEventsForJoin(NetworkStreamer& p_streamer, const uint32_t& p_bufferSize, uint32_t& op_bytesRead);

            bool PlayerExists();

            void OnEvent(Event* p_event) override;

            uint32_t GetLastJoinEventRead() { return m_lastJoinEventRead; }

            uint32_t GetMaximumNumberOfJoinEvents() { return m_maxNumEvents; }


        private:
            PlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~PlayerHandlerClient();

            DoremiEngine::Logging::Logger* m_logger;

            /**
                PlayerID don't think we need it here
            */
            uint32_t m_PlayerID;

            PlayerClient m_player;

            /**
                Last event read in range of 0-N
            */
            uint32_t m_lastJoinEventRead;

            /**
                Maximum events to read in range of 0-N
            */
            uint32_t m_maxNumEvents;
        };
    }
}