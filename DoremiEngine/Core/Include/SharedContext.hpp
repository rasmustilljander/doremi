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

    namespace Memory
    {
        class MemoryModule;
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

    namespace Core
    {
        class SharedContext
        {
            public:
            virtual const std::string GetWorkingDirectory() const = 0;
            virtual const std::string GetPreferenceDirectory() const = 0;

            virtual const Audio::AudioModule& GetAudioModule() const = 0;
            virtual const DoremiEngine& GetCoreModule() const = 0;
            virtual const Graphic::GraphicModule& GetGraphicModule() const = 0;
            virtual const Memory::MemoryModule& GetMemoryModule() const = 0;
            virtual const Network::NetworkModule& GetNetworkModule() const = 0;
            virtual const Physics::PhysicsModule& GetPhysicsModule() const = 0;
            virtual const Script::ScriptModule& GetScriptModule() const = 0;
        };
    }
}