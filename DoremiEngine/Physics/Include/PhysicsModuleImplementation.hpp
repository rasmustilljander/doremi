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
            void Startup() override;
            void Shutdown() override;
            void Update() override;

            virtual ~PhysicsModuleImplementation();

            float ExampleMethod(const float& posx) override;
            void ExampleMethodAltersData(float* p_posX, float* p_posY);

            private:
            const Core::SharedContext& m_sharedContext;
        };
    }
}