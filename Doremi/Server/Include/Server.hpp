#pragma once

namespace Doremi
{
    class ServerMain
    {
        public:
        /**
            Constructor
        */
        ServerMain();

        /**
            Destructor
        */
        virtual ~ServerMain();

        /**

        */
        void Start();


        private:
        void* m_engineModule;
    };
}