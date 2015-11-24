#pragma once
#include <string>
#include <SharedContext.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContextImplementation : public SharedContext
        {
            public:
            SharedContextImplementation()
            : m_workingDirectory(""), m_audio(nullptr), m_core(nullptr),
              m_graphic(nullptr), m_memory(nullptr), m_network(nullptr), m_physics(nullptr),
              m_script(nullptr)
            {
            }

            void SetWorkingDirectory(const std::string& p_workingDirectory)
            {
                m_workingDirectory = p_workingDirectory;
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
            void SetNetworkModule(Network::NetworkModuleInterface* p_networkModule)
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

            Audio::AudioModule& GetAudioModule() const
            {
                if(m_audio != nullptr)
                {
                    return *m_audio;
                }
                throw std::runtime_error("Audiomodule has not been initialized.");
            }

            DoremiEngine& GetCoreModule() const
            {
                if(m_core != nullptr)
                {
                    return *m_core;
                }
                throw std::runtime_error("Coremodule has not been initialized.");
            };

            Graphic::GraphicModule& GetGraphicModule() const
            {
                if(m_graphic != nullptr)
                {
                    return *m_graphic;
                }
                throw std::runtime_error("Graphic module has not been initialized.");
            };

            Memory::MemoryModule& GetMemoryModule() const
            {
                if(m_memory != nullptr)
                {
                    return *m_memory;
                }
                throw std::runtime_error("Memory module has not been initialized.");
            };

            Network::NetworkModuleInterface& GetNetworkModule() const
            {
                if(m_network != nullptr)
                {
                    return *m_network;
                }
                throw std::runtime_error("Network module has not been initialized.");
            };

            Physics::PhysicsModule& GetPhysicsModule() const
            {
                if(m_physics != nullptr)
                {
                    return *m_physics;
                }
                throw std::runtime_error("Physics module has not been initialized.");
            };

            Script::ScriptModule& GetScriptModule() const
            {
                if(m_script != nullptr)
                {
                    return *m_script;
                }
                throw std::runtime_error("Script module has not been initialized.");
            };

            private:
            std::string m_workingDirectory;
            Audio::AudioModule* m_audio;
            DoremiEngine* m_core;
            Graphic::GraphicModule* m_graphic;
            Memory::MemoryModule* m_memory;
            Network::NetworkModuleInterface* m_network;
            Physics::PhysicsModule* m_physics;
            Script::ScriptModule* m_script;
        };
    }
}