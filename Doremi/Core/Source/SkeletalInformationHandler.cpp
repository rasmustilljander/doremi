#pragma once
// Project specific
#include <SkeletalInformationHandler.hpp>
// Loaders
#include <Doremi/Core/Include/LevelLoaderClient.hpp>
// Logging
#include <DoremiEngine/Logging/Include/LoggingModule.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>

// ANimations
#include <DoremiEngine/Graphic/Include/Interface/Animation/SkeletalInformation.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/SkeletalAnimationComponent.hpp>

// Graphics
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SkeletalAnimationManager.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

namespace Doremi
{
    namespace Core
    {
        SkeletalInformationHandler::SkeletalInformationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), m_logger(nullptr)
        {
            m_logger = &m_sharedContext.GetLoggingModule().GetSubModuleManager().GetLogger();
        }

        SkeletalInformationHandler::~SkeletalInformationHandler()
        {
            // Do not delete m_logger, internally handled by loggingmodule
        }

        void SkeletalInformationHandler::StartSkeletalInformationHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new SkeletalInformationHandler(p_sharedContext);
            }
        }

        void SkeletalInformationHandler::StopSkeletalInformationHandler()
        {
            delete m_singleton;
            m_singleton = nullptr;
        }

        std::string SkeletalInformationHandler::CreateFileNameFromEnum(const SkeletalAnimationType& p_type)
        {
            std::string t_fileName;
            switch(p_type)
            {
                case SkeletalAnimationType::MELEENEMY:
                    t_fileName = "MeleeEnemy";
                    break;
                case SkeletalAnimationType::RANGEDENEMY:
                    t_fileName = "Models/SmallRobot12.drm";
                    break;
                case SkeletalAnimationType::PLAYER:
                    t_fileName = "Models/SmallRobot12.drm";
                    break;
                default:
                    t_fileName = "ErrorType in skeletalanimationtype";
                    break;
            }
            return t_fileName;
        }

        SkeletalInformationHandler* SkeletalInformationHandler::m_singleton = nullptr;

        SkeletalInformationHandler* SkeletalInformationHandler::GetInstance() { return m_singleton; }

        LoadedCharacter SkeletalInformationHandler::LoadSkeletalCharacter(const SkeletalAnimationType& p_type)
        {
            /*if (m_loadedCharacters.count(p_type))
            {
                return m_loadedCharacters[p_type];
            }*/
            // else
            //{
            std::string t_fileName = CreateFileNameFromEnum(p_type);
            LoadedCharacter t_loadedCharacter;
            DoremiEngine::Graphic::SkeletalInformation* t_upperInformation =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().CreateSkeletalInformation();
            DoremiEngine::Graphic::SkeletalInformation* t_lowerInformation =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().CreateSkeletalInformation();

            LevelLoaderClient loader = LevelLoaderClient(m_sharedContext);

            CharacterDataNames playerCharData = loader.LoadCharacter("Models/EvenCoolerSuperCoolManBot.drm"); // TODOLH remove when material is in
            t_loadedCharacter.characterData = loader.LoadSkeletalCharacter(t_fileName, t_upperInformation, t_lowerInformation);
            t_loadedCharacter.lowerBody = t_lowerInformation;
            t_loadedCharacter.upperBody = t_upperInformation;
            // m_loadedCharacters[p_type] = t_loadedCharacter;
            return t_loadedCharacter;
            // }
        }

        /*LoadedCharacter SkeletalInformationHandler::LoadSkeletalCharacter(std::string p_fileName)
        {
            if (m_loadedCharacters.count(p_fileName))
            {
                return m_loadedCharacters[p_fileName];
            }
            else
            {
                LoadedCharacter t_loadedCharacter;
                DoremiEngine::Graphic::SkeletalInformation* t_upperInformation =
        m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().CreateSkeletalInformation();
                DoremiEngine::Graphic::SkeletalInformation* t_lowerInformation =
        m_sharedContext.GetGraphicModule().GetSubModuleManager().GetSkeletalAnimationManager().CreateSkeletalInformation();

                LevelLoaderClient loader = LevelLoaderClient(m_sharedContext);

                CharacterDataNames playerCharData = loader.LoadCharacter("Models/EvenCoolerSuperCoolManBot.drm");
                t_loadedCharacter.characterData = loader.LoadSkeletalCharacter(p_fileName, t_upperInformation, t_lowerInformation);
                t_loadedCharacter.lowerBody = t_lowerInformation;
                t_loadedCharacter.upperBody = t_upperInformation;
                m_loadedCharacters[p_fileName] = t_loadedCharacter;
                return t_loadedCharacter;
            }
        }*/
    }
}
