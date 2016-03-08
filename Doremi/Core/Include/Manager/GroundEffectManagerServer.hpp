#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

// Standard libraries
#include <vector>
// Third party libraries
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;
namespace Doremi
{
    namespace Core
    {
        /**
        This manager handles ground effects. It is a work in progress*/
        class GroundEffectManagerServer : public Manager, public Subscriber
        {
        public:
            explicit GroundEffectManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~GroundEffectManagerServer();

            /**
            Updates a local list of positions that particles have collided on*/
            void Update(double p_dt) override;

            /**
            Does nothing at the moment*/
            void OnEvent(Event* p_event) override;

        private:
            // positions of all particle hits
            vector<XMFLOAT3> m_groundEffectPoints;
        };
    }
}