#pragma once

#include <DoremiEngine.hpp>
#include <SharedContext.hpp>

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
            SharedContext &Initialize(const size_t &);

            /**
                TODO docs
            */
            const SharedContext &GetSharedContext() const override
            {
                return *m_sharedContext;
            }

            private:
            SharedContext *m_sharedContext;

            // Loading .dll
            void LoadAudioModule(SharedContext &o_sharedContext);
            void LoadGraphicModule(SharedContext &o_sharedContext);
            void LoadMemoryModule(SharedContext &o_sharedContext);
            void LoadNetworkModule(SharedContext &o_sharedContext);
            void LoadPhysicsModule(SharedContext &o_sharedContext);
            void LoadScriptModule(SharedContext &o_sharedContext);

            // Pointers to .dll processes
            void *m_audioLibrary;
            void *m_graphicLibrary;
            void *m_memoryLibrary;
            void *m_networkLibrary;
            void *m_physicsLibrary;
            void *m_scriptLibrary;

            // Pointers to the interfaces
            Audio::AudioModule *m_audioModule;
            Graphic::GraphicModule *m_graphicModule;
            Memory::MemoryModule *m_memoryModule;
            Network::NetworkModule *m_networkModule;
            Physics::PhysicsModule *m_physicsModule;
            Script::ScriptModule *m_scriptModule;
        };
    }
}
