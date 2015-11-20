#pragma once
#include <string>
#include <SharedContext.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        /**

        */
        class SharedContextImplementation : public SharedContext
        {
            public:
            SharedContextImplementation()
            : m_workingDirectory(""), m_preferenceDirectory(""), m_audio(nullptr), m_core(nullptr),
              m_graphic(nullptr), m_memory(nullptr), m_network(nullptr), m_physics(nullptr),
              m_script(nullptr)
            {
            }

            void SetAudioModule(Audio::AudioModule* p_audioModule)
            {
                m_audio = p_audioModule;
            }
            void SetCoreModule(DoremiEngine* p_coreModule)
            {
                m_core = p_coreModule;
            }
            void SetGraphicModule(Graphic::GraphicModule* p_graphicModule)
            {
                m_graphic = p_graphicModule;
            }
            void SetMemoryModule(Memory::MemoryModule* p_memoryModule)
            {
                m_memory = p_memoryModule;
            }
            void SetNetworkModule(Network::NetworkModule* p_networkModule)
            {
                m_network = p_networkModule;
            }
            void SetPhysicsModule(Physics::PhysicsModule* p_physicsModule)
            {
                m_physics = p_physicsModule;
            }
            void SetScriptModule(Script::ScriptModule* p_scriptModule)
            {
                p_scriptModule = m_script;
            }

            const std::string GetWorkingDirectory() const
            {
                return m_workingDirectory;
            };
            const std::string GetPreferenceDirectory() const
            {
                return m_preferenceDirectory;
            };

            Audio::AudioModule& GetAudioModule() const
            {
                return *m_audio;
            }
            DoremiEngine& GetCoreModule() const
            {
                return *m_core;
            };
            Graphic::GraphicModule& GetGraphicModule() const
            {
                return *m_graphic;
            };
            Memory::MemoryModule& GetMemoryModule() const
            {
                return *m_memory;
            };
            Network::NetworkModule& GetNetworkModule() const
            {
                return *m_network;
            };
            Physics::PhysicsModule& GetPhysicsModule() const
            {
                return *m_physics;
            };
            Script::ScriptModule& GetScriptModule() const
            {
                return *m_script;
            };

            private:
            std::string m_workingDirectory;
            std::string m_preferenceDirectory;
            Audio::AudioModule* m_audio;
            DoremiEngine* m_core;
            Graphic::GraphicModule* m_graphic;
            Memory::MemoryModule* m_memory;
            Network::NetworkModule* m_network;
            Physics::PhysicsModule* m_physics;
            Script::ScriptModule* m_script;
        };
    }
}