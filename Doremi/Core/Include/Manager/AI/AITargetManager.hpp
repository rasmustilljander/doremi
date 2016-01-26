#include <Doremi/Core/Include/Manager/Manager.hpp>
namespace Doremi
{
    namespace Core
    {
        class AITargetManager : public Manager
        {
        public:
            AITargetManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~AITargetManager();
            void Update(double p_dt) override;
        };
    }
}