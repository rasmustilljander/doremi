#pragma once

#include <DoremiEngine.hpp>
#include <Internal/SharedContextImplementation.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class DoremiEngineImplementation : public DoremiEngine
        {
            public:
            /**
                Constructor
            */
            DoremiEngineImplementation();

            /**
                Destructor
            */
            virtual ~DoremiEngineImplementation();

            /**
                TODO docs
            */
            SharedContext& Initialize(const size_t&);

            /**
                TODO docs
            */
            const SharedContext& GetSharedContext() const override
            {
                return *static_cast<SharedContext*>(m_sharedContext);
            }

            private:
            void BuildWorkingDirectory(SharedContextImplementation& o_sharedContext);

            SharedContextImplementation* m_sharedContext;

            // Loading .dll
            void LoadAudioModule(SharedContextImplementation& o_sharedContext);
            void LoadGraphicModule(SharedContextImplementation& o_sharedContext);
            void LoadMemoryModule(SharedContextImplementation& o_sharedContext);
            void LoadNetworkModule(SharedContextImplementation& o_sharedContext);
            void LoadPhysicsModule(SharedContextImplementation& o_sharedContext);
            void LoadScriptModule(SharedContextImplementation& o_sharedContext);

            // Pointers to .dll processes
            void* m_audioLibrary;
            void* m_graphicLibrary;
            void* m_memoryLibrary;
            void* m_networkLibrary;
            void* m_physicsLibrary;
            void* m_scriptLibrary;

            // Pointers to the interfaces
            Audio::AudioModule* m_audioModule;
            Graphic::GraphicModule* m_graphicModule;
            Memory::MemoryModule* m_memoryModule;
            Network::NetworkModule* m_networkModule;
            Physics::PhysicsModule* m_physicsModule;
            Script::ScriptModule* m_scriptModule;
        };
    }
}
