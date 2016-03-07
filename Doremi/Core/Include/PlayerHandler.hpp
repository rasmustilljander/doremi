#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>
#include <map>
#include <DirectXMath.h>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <Doremi/Core/Include/PlayerClasses/GunController.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>


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
        struct EntityCreatedEvent;
        struct RemoveEntityEvent;
        struct PlayerRespawnEvent;


        /**
            TODOCM doc
        */
        struct Player
        {
            Player(EntityID p_EntityID, InputHandler* p_inputHandler, FrequencyBufferHandler* p_frequencyBufferHandler)
                : m_playerEntityID(p_EntityID),
                  m_moveSpeed(0.8f),
                  m_autoRetardation(50.0f),
                  m_inputHandler(p_inputHandler),
                  m_frequencyBufferHandler(p_frequencyBufferHandler),
                  m_turnSpeed(0.01f)
            {
            }
            ~Player()
            {

                delete m_inputHandler;
                delete m_frequencyBufferHandler;
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
            FrequencyBufferHandler* m_frequencyBufferHandler;

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
            float m_turnSpeed;
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
                TODOCM doc
            */
            virtual void Update(double p_dt) = 0;


        protected:
            /**
                TODOEA doc
            */
            explicit PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOEA doc
            */
            ~PlayerHandler();

            /**
                TODOEA doc
            */
            void UpdatePlayerPositions(Player* p_player);

            /**
                TODOCM doc
            */
            void UpdateFiring(Player* p_player);

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
        };
    }
}
