#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

// standard
#include <string>
#if defined(_WINDLL)
#define CONFIGURATION_DLL_EXPORT __declspec(dllexport)
#else
#define CONFIGURATION_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Configuration
    {
        /**
        A struct containing all the information gathered from config files.
        */
        struct ConfiguartionInfo
        {
            // Graphic stuff
            float ScreenWidth = 800;
            float ScreenHeight = 600;
            float CameraDistanceFromPlayer = 5.0f;
            int CameraViewDistance = 1000;
            int CameraFieldOfView = 60;
            int Fullscreen = 0;

            // Audio stuff
            float MasterVolume = 0.5f;
            float EffectVolume = 1.0f;
            float MusicVolume = 1.0f;

            // Network stuff
            int UseIPMaster = 1;
            std::string URLMaster = "http://www.doremithegame.com/";
            std::string IPToMaster = "127.0.0.1";
            int PortMasterClient = 3200;
            int PortMasterServer = 3201;
            int PortServerConnecting = 5050;
            int PortServerConnected = 4050;
            std::string ServerName = "DefaultServer";
            int MaxPlayers = 16;


            std::string IPToServer = "127.0.0.1";
            uint32_t LastServerPlayerID = 0;

            // Key binds TODOKO ask how this works
            int Forward = 119;
            int Backward = 115;
            int Left = 97;
            int Right = 100;
            int LeftClick = 1;
            int RightClick = 3;
            int StartRepeatingAudioRecording = 228;
            int PlayRepeatableAudioRecording = 246;
            int ExitGame = 27;

            // AI stuff
            float AIJumpDistance = 3.0f;
            float AIAimOffset = 0.3f;
            float MeleeEnemySpeed = 50;
            float RangedEnemySpeed = 45;

            // Player specific
            float TurnSpeed = 0.01f;
            float MaxPitch = 1;
            float MinPitch = -1;
            float PlayerSpeed = 45;
            float JumpPower = 1;
            bool FriendlyFire = false;
        };
        /**
        Reads and saves configuration from file. If another module needs configuration values they can use fucntions in this class to get them.
        */
        class ConfigurationModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            Returns a non modifiable struct with all configuration values
            */
            virtual const ConfiguartionInfo& GetAllConfigurationValues() const = 0;

            /**
            Reads values from the specified file and saves the values recognized
            */
            virtual void ReadConfigurationValuesFromFile(const std::string& p_fileName) = 0;

            /**
            Writes the configuration values to file. If the file have the config value it will be overwriten
            Every value the specified filed doesnt have will be writen at the end of the file
            */
            virtual void WriteConfigurationValuesToFile(const std::string& p_fileName) = 0;

            /**
            CAUTION!!! Gives full control of the configuration information. Dont delete outside!!
            Usable when you want to change values in the configuration that later should be saved
            */
            virtual ConfiguartionInfo& GetModifiableConfigurationInfo() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Configuration::ConfigurationModule* (*CREATE_CONFIGURATION_MODULE)(const DoremiEngine::Core::SharedContext&);
CONFIGURATION_DLL_EXPORT DoremiEngine::Configuration::ConfigurationModule* CreateConfigurationModule(const DoremiEngine::Core::SharedContext& p_context);
}