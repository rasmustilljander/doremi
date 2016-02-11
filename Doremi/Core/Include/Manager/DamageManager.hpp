#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Is in controll of seeing if a hit should produce damage or not, if it should it'll push a event to the managers that is in charge of removing
        health. This manager is also in charge of removing the enemies bullets
        */
        class DamageManager : public Manager , public Subscriber
        {
        public:
            DamageManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~DamageManager();
            void Update(double p_dt) override;
            void OnEvent(Event* p_event) override;
        };
    }
}