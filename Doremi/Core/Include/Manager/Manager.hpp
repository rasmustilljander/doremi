#pragma once
// Project specific

namespace Doremi
{
    namespace Core
    {
        /**
        Different types of managers govern how the game is run.
        Each manager has its update method run once per game loop.
        Managers have to be put in the vector<Manager*> in GameCore
        for it to run*/
        class Manager
        {
            public:
            Manager();
            ~Manager();

            /** Big update loop for every type of manager*/
            virtual void Update(double p_dt) = 0;

            protected:
        };
    }
}