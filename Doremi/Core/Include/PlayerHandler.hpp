#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>


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
        class PlayerHandler
        {
            public:
            /** Is a singleton. Use this method to get the EventManager*/
            static PlayerHandler* GetInstance();
            PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~PlayerHandler();
            static void StartPlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            void Initialize(int m_playerEntityID);
            void UpdatePosition();
            const int& GetPlayerEntityID() const { return m_playerEntityID; }


            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            InputHandler* m_inputHandler;

            static PlayerHandler* m_singleton;


            int m_playerEntityID;

            float m_moveSpeed = 3;
            float m_autoRetardation = 0.8;
            float m_maxSpeed = 10;


            // int m_bodyID;
            // int m_materialID;
        };
    }
}
