#pragma once
#include <PhysicsModule.hpp>

namespace DoremiEngine
{
    namespace Physics
    {
        class PhysicsModuleImplementation : public PhysicsModule
        {
        public:
            PhysicsModuleImplementation();
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
        };
    }
}