#pragma once

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
        struct SharedContext
        {
            const char* workingDirectory;
            const char* preferenceDirectory;
            Audio::AudioModule* sound;
            DoremiEngine* core;
            Graphic::GraphicModule* graphic;
            Memory::MemoryModule* memory;
            Network::NetworkModule* network;
            Physics::PhysicsModule* physics;
            Script::ScriptModule* script;

            SharedContext()
            : workingDirectory(nullptr), preferenceDirectory(nullptr), sound(nullptr), core(nullptr),
              graphic(nullptr), memory(nullptr), network(nullptr), physics(nullptr), script(nullptr)
            {
            }
        };
    }
}