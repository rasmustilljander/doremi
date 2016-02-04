#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>
#include <map>
#include <DirectXMath.h>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <Doremi/Core/Include/PlayerClasses/GunController.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>


namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class InputHandler;
        class NetworkEventSender;
        class FrequencyBufferHandler;
        class NetworkPriorityHandler;


        /**
            TODOCM doc
        */
        struct Player
        {
            Player(EntityID p_EntityID, InputHandler* p_inputHandler, NetworkEventSender* p_networkEventSender,
                   FrequencyBufferHandler* p_frequencyBufferHandler, NetworkPriorityHandler* p_networkPriorityHandler)
                : m_playerEntityID(p_EntityID),
                  m_moveSpeed(0.3f),
                  m_autoRetardation(50.0f),
                  m_inputHandler(p_inputHandler),
                  m_networkEventSender(p_networkEventSender),
                  m_frequencyBufferHandler(p_frequencyBufferHandler),
                  m_networkPriorityHandler(p_networkPriorityHandler),
                  m_turnSpeed(0.01f)
            {
            }
            ~Player()
            {
                delete m_networkEventSender;
                delete m_inputHandler;
                delete m_frequencyBufferHandler;
                delete m_networkPriorityHandler;
            }

            /**
                TODOEA doc
            */
            EntityID m_playerEntityID;

            /**
                TODOEA doc
            */
            InputHandler* m_inputHandler;

            /**
                TODOCM doc
            */
            NetworkEventSender* m_networkEventSender;

            /**
                TODOCM doc
            */
            FrequencyBufferHandler* m_frequencyBufferHandler;

            /**
                TODOCM doc
            */
            NetworkPriorityHandler* m_networkPriorityHandler;

            /**
                TODOEA doc
            */
            float m_moveSpeed;

            /**
                TODOEA doc
            */
            float m_autoRetardation;

            /**
                TODOEA doc
            */
            float m_maxSpeed;

            /**
                TODOEA doc
            */
            float m_turnSpeed;
        };

        // Temporary
        struct AddRemoveStruct
        {
            AddRemoveStruct(bool p_AddNotRemove, uint32_t p_IDOrBlueprint) : AddNotRemove(p_AddNotRemove), EntityIDOrBlueprint(p_IDOrBlueprint) {}
            bool AddNotRemove;
            uint32_t EntityIDOrBlueprint;
        };


        /**
            TODOEA doc
        */
        class PlayerHandler : public Subscriber
        {
        public:
            /**
                Is a singleton. Use this method to get the EventManager
            */
            static PlayerHandler* GetInstance();

            /**
                TODOEA doc
            */
            PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOEA doc
            */
            ~PlayerHandler();

            /**
                TODOEA doc
            */
            static void StartPlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOEA doc
            */
            InputHandler* GetDefaultInputHandler();

            /**
                TODOCM doc
            */
            FrequencyBufferHandler* GetDefaultFrequencyBufferHandler();

            /**
                TODOCM doc
            */
            FrequencyBufferHandler* GetFrequencyBufferHandlerForPlayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            NetworkPriorityHandler* GetNetworkPriorityHandlerForplayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            bool GetDefaultPlayerEntityID(EntityID& o_outID);

            /**
                TODOCM doc
            */
            bool GetEntityIDForPlayer(uint32_t p_playerID, EntityID& p_entityID);

            /**
                TODOCM doc
            */
            InputHandler* GetInputHandlerForPlayer(uint32_t p_playerID);

            /**
                TODOCM doc
            */
            NetworkEventSender* GetNetworkEventSenderForPlayer(uint32_t p_playerID);

            /**
                TODOCM doc
                TODOCM maybe change the way a player is created due to the need of external creation of inputHandler cause of difference in client and
               server
            */
            void CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler);

            /**
                TODOCM doc
            */
            void UpdateClient();

            /**
                TODOCM doc
            */
            void UpdateServer(double p_dt);

            /**
                TODOCM doc
            */
            void UpdateNetworkObjectPriority(double p_dt);

            /**
                TODOCM doc
            */
            void QueueAddObjectToPlayers(EntityID p_ID, uint32_t p_blueprint, DirectX::XMFLOAT3 p_position);

            /**
                TODOCM doc
            */
            void QueueRemoveObjectToPlayers(uint32_t p_entityID);

            /**
                TODOCM doc
            */
            void UpdateAddRemoveObjects();

            /**
                TODOCM doc
            */
            void UpdatePlayerInputsClient();

            /**
                TODOCM doc
            */
            void UpdatePlayerInputsServer();

            /**
                TODOCM doc
            */
            uint32_t GetNumOfPlayers();

            /**
                Returns the map of players
            */
            std::map<uint32_t, Player*>& GetPlayerMap();

            /**
                TODOCM doc
            */
            void RemoveAllPlayers();

            /**
                TODOCM doc
            */
            void AddNetObjectToPlayers(const EntityID& p_entityID);

            /**
                TODOCM doc
            */
            void OnEvent(Event* p_event) override;

        private:
            /**
                TODOEA doc
            */
            void UpdatePlayerPositions();

            /**
                TODOCM doc
            */
            void UpdatePlayerRotationsClient();

            /**
                TODOCM doc
            */
            void UpdatePlayerRotationsServer();

            /**
                TODOCM doc
            */
            void UpdateFiring();

            /**
                TODOCM doc
            */
            static PlayerHandler* m_singleton;

            /**
                TODOEA doc
            */
            const DoremiEngine::Core::SharedContext& m_sharedContext;


            /**
                TODOEA doc
            */
            GunController m_gunController;

            /**
                Map from playerID to player struct, playerID != EntityID
            */
            std::map<uint32_t, Player*> m_playerMap;

            /**
                TODOCM doc
            */
            std::vector<AddRemoveStruct> m_sinceGameStartAddRemoves;


            // TODOEA add these attributes?
            // int m_bodyID;
            // int m_materialID;
        };
    }
}
