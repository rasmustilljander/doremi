#pragma once


namespace Doremi
{
    namespace Core
    {
        enum class ServerStates
        {
            LOBBY,
            IN_GAME,
        };

        class ServerStateHandler
        {
        public:
            static ServerStateHandler* GetInstance();

            ServerStates GetState() { return m_state; }

        private:
            ServerStateHandler();

            ~ServerStateHandler();

            static ServerStateHandler* m_singleton;

            ServerStates m_state;
        };
    }
}