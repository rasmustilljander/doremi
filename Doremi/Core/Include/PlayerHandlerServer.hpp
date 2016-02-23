#pragma once
#include <Doremi/Core/Include/PlayerHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        class NetworkEventSender;
        class NetworkPriorityHandler;
        struct GunFireToggleEvent;
        class NetworkStreamer;
        struct SetHealthEvent;
        struct SetTransformEvent;
        struct AnimationTransitionEvent;

        /**
            TODOCM doc
        */
        struct PlayerServer : public Player
        {
            PlayerServer(EntityID p_EntityID, InputHandler* p_inputHandler, FrequencyBufferHandler* p_frequencyBufferHandler,
                         NetworkPriorityHandler* p_networkPriorityHandler, NetworkEventSender* p_networkEventSender)
                : Player(p_EntityID, p_inputHandler, p_frequencyBufferHandler),
                  m_networkPriorityHandler(p_networkPriorityHandler),
                  m_networkEventSender(p_networkEventSender),
                  m_StartEvent(0),
                  m_EndEvent(0)

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

            /**
                TODOCM doc
            */
            uint32_t m_StartEvent;

            /**
                TODOCM doc
            */
            uint32_t m_EndEvent;
        };

        /**
            TODOCM doc
        */
        struct InactivePlayerServer
        {
        public:
            InactivePlayerServer() {}

            ~InactivePlayerServer() {}
            /**
                TODOCM doc
            */
            PlayerServer* m_savedPlayer;

            /**
                TODOCM DOC
            */
            DirectX::XMFLOAT3 m_savedPosition;

            /**
                TODOCM DOC
            */
            DirectX::XMFLOAT4 m_savedOrientation;

            /**
                TODOCM doc
            */
            float m_savedHealth;
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

            void RemoveAndSavePlayer(uint32_t p_playerID);

            bool InactivePlayerIDExists(const uint32_t& p_playerID);

            bool ActivePlayerIDExists(const uint32_t& p_playerID);

            uint32_t GetMaxEventForPlayer(uint32_t p_playerID);

            NetworkEventSender* GetNetworkEventSenderForPlayer(uint32_t p_playerID);

            NetworkPriorityHandler* GetNetworkPriorityHandlerForplayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            void UpdateNetworkObjectPriority(double p_dt);

            /**
                TODOCM doc
            */
            void SetupRejoin(uint32_t t_playerID);

            bool UpdateRejoinQueueForPlayer(uint32_t p_EventSlot, uint32_t t_playerID);


            /**
                TODOCM doc
            */
            void WriteQueuedEventsFromLateJoin(NetworkStreamer& p_streamer, const uint32_t& p_bufferSize, uint32_t& op_bytesWritten, uint32_t t_playerID);

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

            /**
                TODOCM doc
            */
            void QueuePlayerRespawnEventToPlayers(PlayerRespawnEvent* p_playerRespawn);

            void QueueGunFireToggleEventToPlayers(GunFireToggleEvent* t_gunFireToggleEvent);

            /**
                TODOCM doc
            */
            void PlayerHandlerServer::QueueSetHealthEventToPlayers(SetHealthEvent* t_setHealthEvent);

            /**
                TODOCM doc
            */
            void QueueSetTransformEventToPlayers(SetTransformEvent* t_setTransformEvent);

            /**
                TODOLH doc
            */
            void QueueAnimationTransitionToPlayers(AnimationTransitionEvent* t_setTransformEvent);

            /**
                TODOCM doc
            */
            std::vector<Event*> m_lateJoinEventQueue;

            /**
                TODOCM doc
            */
            std::map<uint32_t, InactivePlayerServer*> m_inactivePlayers;
        };
    }
}