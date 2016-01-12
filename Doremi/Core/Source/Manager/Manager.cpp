// Project specific
#include <Manager/Manager.hpp>
#include <utility>
namespace Doremi
{
    namespace Core
    {
        Manager::Manager(const DoremiEngine::Core::SharedContext& p_sharedContext, const std::string& p_name)
            : m_sharedContext(p_sharedContext), m_name(std::move(p_name))
        {
        }
    }
}