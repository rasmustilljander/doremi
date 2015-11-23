#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>

// Standard libraries
#include <vector>
namespace Doremi
{
    namespace Core
    {
        class EntityInterface;
        class GameCore
        {
        public:
            GameCore();
            virtual ~GameCore();
            void Initialize();
            void StartCore();

        private:
            std::vector<Manager*> m_managers;
        };
    }
}