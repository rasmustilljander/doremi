#include <Manager/Manager.hpp>

// Logger
#include <DoremiEngine/Logging/Include/LoggingModule.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>

namespace Doremi
{
    namespace Core
    {
        Manager::Manager(const DoremiEngine::Core::SharedContext& p_sharedContext, const std::string& p_name)
            : m_sharedContext(p_sharedContext), m_name(std::move(p_name))
        {
            using namespace DoremiEngine::Logging;
            using namespace Utilities::Logging;
            Logger& logger = p_sharedContext.GetLoggingModule().GetSubModuleManager().GetLogger();
            logger.LogText(LogTag::GAME, LogLevel::INFO, "Creating manager: %s", m_name.c_str());
        }
    }
}