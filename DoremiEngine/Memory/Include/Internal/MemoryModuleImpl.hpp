#pragma once
#include <MemoryModule.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }

    namespace Memory
    {
        class MemoryModuleImplementation : public MemoryModule
        {
            public:
            /**
                TODO docs
            */
            MemoryModuleImplementation(const Core::SharedContext& p_sharedContext);
            /**
                TODO docs
            */
            virtual ~MemoryModuleImplementation();

            /**
                TODO docs
            */
            void Startup() override;

            /**
                TODO docs
            */
            void Shutdown() override;

            private:
            const Core::SharedContext& m_sharedContext;
        };
    }
}
