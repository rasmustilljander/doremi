#pragma once

namespace Doremi
{
    class GameMain
    {
        public:
        /**
            Constructor
        */
        GameMain();

        /**
            Destructor
        */
        virtual ~GameMain();

        /**

        */
        void Start();


        private:
        void* m_engineModule;
    };
}