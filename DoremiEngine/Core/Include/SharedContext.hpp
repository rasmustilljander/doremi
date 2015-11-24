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
        class NetworkModuleInterface;
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

            virtual Audio::AudioModule& GetAudioModule() const = 0;
            virtual DoremiEngine& GetCoreModule() const = 0;
            virtual Graphic::GraphicModule& GetGraphicModule() const = 0;
            virtual Memory::MemoryModule& GetMemoryModule() const = 0;
            virtual Network::NetworkModuleInterface& GetNetworkModule() const = 0;
            virtual Physics::PhysicsModule& GetPhysicsModule() const = 0;
            virtual Script::ScriptModule& GetScriptModule() const = 0;
        };
    }
}