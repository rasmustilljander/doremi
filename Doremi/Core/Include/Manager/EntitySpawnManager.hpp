#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

// Pre-declaration. Included in .cpp
// For some reason the blueprints are not inside the Doremi::core namespace
enum class Blueprints : uint32_t;

namespace Doremi
{
    namespace Core
    {
        /**
        Manager to create entities. So far it does it either after a period
        of time has elapsed, or after a certain event has been sent. Should
        most likely be expanded in the future.*/
        class EntitySpawnManager : public Manager, public Subscriber
        {
        public:
            EntitySpawnManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~EntitySpawnManager();
            void Update(double p_dt) override;
            void OnEvent(Event* p_event) override;

        private:
            /*
            Big method for specific code depending on which entity we
            want to spawn*/
            void CreateEntity(Blueprints p_blueprint);
        };
    }
}
