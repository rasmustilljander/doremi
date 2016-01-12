#pragma once
// Project specific
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <string>

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
            Manager(const DoremiEngine::Core::SharedContext& p_sharedContext, const std::string& p_name);

            /** Big update loop for every type of manager*/
            virtual void Update(double p_dt) = 0;

            const std::string& GetName() const { return m_name; }

            protected:
            /** Engine-context form which specific interfaces can be accessed*/
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            std::string m_name;
        };
    }
}