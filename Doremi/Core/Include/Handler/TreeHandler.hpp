#pragma once
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <DirectXMath.h>
#include <vector>
#include <cstdint>

namespace Doremi
{
    namespace Core
    {
        class TreeHandler : public Subscriber
        {
        public:
            static TreeHandler* GetInstance();

            void Update();

            void OnEvent(Event* p_event) override;

        private:
            TreeHandler();
            ~TreeHandler();

            static TreeHandler* m_singleton;
        };
    }
}
