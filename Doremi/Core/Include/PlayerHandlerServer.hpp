#pragma once
#include <Doremi/Core/Include/PlayerHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        class NetworkEventSender;
        class NetworkPriorityHandler;

        /**
            TODOCM doc
        */
        struct PlayerServer : public Player
        {
            PlayerServer(EntityID p_EntityID, InputHandler* p_inputHandler, FrequencyBufferHandler* p_frequencyBufferHandler,
                         NetworkPriorityHandler* p_networkPriorityHandler, NetworkEventSender* p_networkEventSender)
                : Player(p_EntityID, p_inputHandler, p_frequencyBufferHandler),
                  m_networkPriorityHandler(p_networkPriorityHandler),
                  m_networkEventSender(p_networkEventSender)
            {
            }
            ~PlayerServer()
            {
                delete m_networkPriorityHandler;
                delete m_networkEventSender;
            }

            /**
                TODOCM doc
            */
            NetworkPriorityHandler* m_networkPriorityHandler;


            /**
                TODOCM doc
            */
            NetworkEventSender* m_networkEventSender;
        };

        /**
            TODOCM doc
        */
        class PlayerHandlerServer : public PlayerHandler, public Subscriber
        {
        public:
            PlayerHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~PlayerHandlerServer();

            static void StartPlayerHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void Update(double p_dt) override;

            void UpdatePlayerInputs() override;

            void CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler) override;

            NetworkEventSender* GetNetworkEventSenderForPlayer(uint32_t p_playerID);

            NetworkPriorityHandler* GetNetworkPriorityHandlerForplayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            void UpdateNetworkObjectPriority(double p_dt);

            /**
                TODOCM doc
            */
            void OnEvent(Event* p_event) override;

        private:
            /**
                TODOCM doc
            */
            void AddNetObjectToPlayers(const EntityID& p_entityID);

            /**
                TODOCM doc
            */
            void QueueEntityCreatedEventToPlayers(EntityCreatedEvent* p_entityCreatedEvent);


            /**
                TODOCM doc
            */
            void QueueRemoveEntityEventToPlayers(RemoveEntityEvent* p_removeEvent);
        };
    }
}