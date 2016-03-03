#pragma once
#include <string>

namespace DoremiEngine
{
    namespace Audio
    {
        class AudioModule;
    }

    namespace Core
    {
        class DoremiEngine;
    }

    namespace Graphic
    {
        class GraphicModule;
    }

    namespace Network
    {
        class NetworkModule;
    }

    namespace Physics
    {
        class PhysicsModule;
    }

    namespace Script
    {
        class ScriptModule;
    }

    namespace Input
    {
        class InputModule;
    }

    namespace AI
    {
        class AIModule;
    }

    namespace Logging
    {
        class LoggingModule;
    }

    namespace Configuration
    {
        class ConfigurationModule;
    }

    namespace Core
    {
        class SharedContext
        {
        public:
            virtual const std::string GetWorkingDirectory() const = 0;

            virtual Audio::AudioModule& GetAudioModule() const = 0;
            virtual DoremiEngine& GetCoreModule() const = 0;
            virtual Graphic::GraphicModule& GetGraphicModule() const = 0;
            virtual Network::NetworkModule& GetNetworkModule() const = 0;
            virtual Physics::PhysicsModule& GetPhysicsModule() const = 0;
            virtual Script::ScriptModule& GetScriptModule() const = 0;
            virtual Input::InputModule& GetInputModule() const = 0;
            virtual AI::AIModule& GetAIModule() const = 0;
            virtual Logging::LoggingModule& GetLoggingModule() const = 0;
            virtual Configuration::ConfigurationModule& GetConfigurationModule() const = 0;
            virtual void RequestApplicationExit() const = 0;
        };
    }
}