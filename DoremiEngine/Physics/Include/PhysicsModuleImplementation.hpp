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
            void SetWorkingDirectory(const std::string& p_workingDirectory) override;

            /**
            TODO
            */
            void Shutdown() override;

            float DoSomething(const float& posx) override;
        };
    }
}