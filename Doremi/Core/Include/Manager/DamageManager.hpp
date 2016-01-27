#include <Doremi/Core/Include/Manager/Manager.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Is in controll of seeing if a hit should produce damage or not, if it should it'll push a event to the managers that is in charge of removing
        health. This manager is also in charge of removing the enemies bullets
        */
        class DamageManager : public Manager
        {
        public:
            DamageManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~DamageManager();
            void Update(double p_dt) override;
        };
    }
}