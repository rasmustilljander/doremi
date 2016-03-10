#pragma once
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>
#include <Doremi/Core/Include/NetworkEventSender.hpp>

namespace Doremi
{
    namespace Core
    {
        class InputHandlerServer;
        struct GunFireToggleEvent;
        class NetworkStreamer;
        struct SetHealthEvent;
        struct SetTransformEvent;
        struct AnimationTransitionEvent;
        struct DamageTakenEvent;
        struct EmptyEvent;

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
            InactivePlayerServer()
                : m_savedPlayer(nullptr), m_savedPosition(DirectX::XMFLOAT3(0, 0, 0)), m_savedOrientation(DirectX::XMFLOAT4(0, 0, 0, 0)), m_savedHealth(0)
            {
            }

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
            static void StartPlayerHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            std::map<uint32_t, PlayerServer*>& GetPlayerMap();

            bool GetEntityIDForPlayer(uint32_t p_playerID, EntityID& outID);

            InputHandlerServer* GetInputHandlerForPlayer(uint32_t p_playerID);

            FrequencyBufferHandler* GetFrequencyBufferHandlerForPlayer(uint32_t p_playerID);

            bool IsPlayer(EntityID p_entityID);

            void Update(double p_dt) override;

            void UpdatePlayerInputs(Player* t_player);

            void UpdatePlayerRotations(Player* t_player);

            void CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler);

            void RemoveAndSavePlayer(uint32_t p_playerID);

            bool InactivePlayerIDExists(const uint32_t& p_playerID);

            bool ActivePlayerIDExists(const uint32_t& p_playerID);

            uint32_t GetMaxEventForPlayer(uint32_t p_playerID);

            NetworkEventSender* GetNetworkEventSenderForPlayer(uint32_t p_playerID);

            NetworkPriorityHandler* GetNetworkPriorityHandlerForplayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            void UpdateNetworkObjectPriority(Player* t_player, double p_dt);

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
            explicit PlayerHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~PlayerHandlerServer();

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
            TODOKO doc
            */
            void QueueDamageEventToPlayers(DamageTakenEvent* t_takeDamageEvent);

            /**
            TODOKO doc
            */
            void QueueChangedCheckpointEventToPlayers(EmptyEvent* t_changeCheckpointEvent);

            /**
            Debug method to change spawn point*/
            void HandleChangeOfSpawnPoint(Player* p_player);

            /**
                TODOCM doc
            */
            std::vector<Event*> m_lateJoinEventQueue;

            /**
                TODOCM doc
            */
            std::map<uint32_t, InactivePlayerServer*> m_inactivePlayers;

            /**
                Map from playerID to player struct, playerID != EntityID
            */
            std::map<uint32_t, PlayerServer*> m_playerMap;
        };
    }
}