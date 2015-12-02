#pragma once
#include <PhysicsModule.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }

    namespace Physics
    {
        class PhysicsModuleImplementation : public PhysicsModule
        {
            public:
            PhysicsModuleImplementation(const Core::SharedContext& p_sharedContext);
            virtual ~PhysicsModuleImplementation();

            /**
            TODO
            */
            void Startup() override;

            /**
            TODO
            */
            void Shutdown() override;

            float ExampleMethod(const float& posx) override;
            void ExampleMethodAltersData(float* p_posX, float* p_posY);

            private:
            const Core::SharedContext& m_sharedContext;
        };
    }
}