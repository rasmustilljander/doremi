// Project specific
#include <Manager/Manager.hpp>
namespace Doremi
{
    namespace Core
    {
        Manager::Manager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            :m_sharedContext(p_sharedContext)
        {

        }
        Manager::~Manager()
        {
        }
    }
}