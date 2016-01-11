#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>
#include <map>
#include <DirectXMath.h>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>


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

        /**
            TODOCM doc
        */
        struct Player
        {
            Player(InputHandler* p_inputHandler) : m_moveSpeed(0.2f), m_autoRetardation(50.0f), m_inputHandler(p_inputHandler), m_turnSpeed(0.01f) {}

            /**
                TODOEA doc
            */
            int m_playerEntityID;

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

            /**
                TODOEA doc
            */
            InputHandler* m_inputHandler;
        };

        /**
            TODOEA doc
        */
        class PlayerHandler
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
            EntityID GetDefaultPlayerEntityID();

            /**
                TODOCM doc
            */
            bool GetEntityIDForPlayer(uint32_t p_playerID, EntityID &p_entityID);

            /**
                TODOCM doc
            */
            InputHandler* GetInputHandlerForPlayer(uint32_t p_playerID);

            /**
                TODOCM doc
                TODOCM maybe change the way a player is created due to the need of external creation of inputHandler cause of difference in client and
               server
            */
            void CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler);

            /**
                TODOCM doc
            */
            void UpdatePlayerInputs();

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

            private:
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

            /**
                Map from playerID to player struct, playerID != EntityID
            */
            std::map<uint32_t, Player*> m_playerMap;

            // TODOEA add these attributes?
            // int m_bodyID;
            // int m_materialID;
        };
    }
}
