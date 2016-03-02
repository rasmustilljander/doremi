#pragma once
// Standard Libraries
#include <unordered_map>
#include <map>
#include <vector>
#include <DirectXMath.h>


namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }

    namespace Logging
    {
        class Logger;
    }
    namespace Graphic
    {
        class SkeletalInformation;
    }
}

namespace Doremi
{
    namespace Core
    {
        struct LoadedCharacter;
        class InputHandler;
        enum class SkeletalAnimationType;
        /**
        Handles recording and analysis. Also handles background sound depending on game state
        */
        class SkeletalInformationHandler
        {
        public:
            /** Is a singleton. Use this method to get the AudioHandler*/
            static SkeletalInformationHandler* GetInstance();
            static void StartSkeletalInformationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static void StopSkeletalInformationHandler();
            std::string CreateFileNameFromEnum(const SkeletalAnimationType& p_type);
            SkeletalInformationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            // LoadedCharacter LoadSkeletalCharacter(std::string p_fileName);
            LoadedCharacter& LoadSkeletalCharacter(const SkeletalAnimationType& p_type);
            ~SkeletalInformationHandler();


        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            std::map<SkeletalAnimationType, LoadedCharacter> m_loadedCharacters;
            static SkeletalInformationHandler* m_singleton;
            // Logger
            DoremiEngine::Logging::Logger* m_logger;
        };
    }
}
