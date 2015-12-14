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
            DirectX::XMFLOAT3 UpdatePosition();

            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            InputHandler* m_inputHandler;

            static PlayerHandler* m_singleton;

            int m_playerEntityID;
            int m_bodyID;
            int m_materialID;
        };
    }
}
