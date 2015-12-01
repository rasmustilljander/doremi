#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define GRAPHIC_DLL_EXPORT __declspec(dllexport)
#else
#define GRAPHIC_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Graphic
    {
        /**
        TODO docs
        */
        class GraphicModule : public DoremiEngine::Core::EngineModule
        {
            public:
            /**
            TODO remove this test method
            */
            virtual float DoFunction(float a, float b) = 0;

            /**
            TODO docs
            */
            virtual void Startup() = 0;

            /**
            TODO docs
            */
            virtual void SetWorkingDirectory(const std::string& p_workingDirectory) = 0;

            /**
            TODO docs
            */
            virtual void Shutdown() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Graphic::GraphicModule* (*CREATE_GRAPHIC_MODULE)(const DoremiEngine::Core::SharedContext&);
GRAPHIC_DLL_EXPORT DoremiEngine::Graphic::GraphicModule*
CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_context);
}