#pragma once
#include <cstdint>
#include <Utility/Utilities/Include/Constants/LoggerConstants.hpp> // TODORT can move to cpp
#include <Utility/Utilities/Include/String/StringHelper.hpp> // TODORT can move to cpp
#include <string> // TODORT can move to cpp

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            /**
            TODORT, can move to cpp
            TODORT, docs
            */
            static std::string BuildFileMapName(const uint32_t& p_uniqueId)
            {
                using namespace std;
                return string(Constants::IPC_DEFAULT_FILEMAP_NAME + to_string(p_uniqueId));
            }

            /**
            TODORT, can move to cpp
            TODORT, docs
            */
            static std::string BuildLogFileName(const std::string& p_consoleName)
            {
                using namespace std;
                using namespace Doremi::Utilities::String;
                struct tm now;
                time_t time_create = time(NULL);
                localtime_s(&now, &time_create);

                string fileName = string("logs/");
                fileName.append(intToStringAtleastTwoDigits(now.tm_year + 1900) + "-");
                fileName.append(intToStringAtleastTwoDigits(now.tm_mon + 1) + "-");
                fileName.append(intToStringAtleastTwoDigits(now.tm_mday) + "_");
                fileName.append(intToStringAtleastTwoDigits(now.tm_hour) + ".");
                fileName.append(intToStringAtleastTwoDigits(now.tm_min));
                fileName.append("__" + p_consoleName);
                fileName.append(".txt");
                return fileName;
            }

            static std::string BuildFolderNameBasedOnTime()
            {
                using namespace std;
                using namespace Doremi::Utilities::String;
                struct tm now;
                time_t time_create = time(NULL);
                localtime_s(&now, &time_create);

                string fileName = "logs/";
                fileName.append(intToStringAtleastTwoDigits(now.tm_year + 1900) + "-");
                fileName.append(intToStringAtleastTwoDigits(now.tm_mon + 1) + "-");
                fileName.append(intToStringAtleastTwoDigits(now.tm_mday) + "_");
                fileName.append(intToStringAtleastTwoDigits(now.tm_hour) + ".");
                fileName.append(intToStringAtleastTwoDigits(now.tm_min));
                return fileName;
            }
        }
    }
}
