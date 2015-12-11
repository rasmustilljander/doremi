#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DirectXMath.h> //TODO Review if we should use Direct x math
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
        Handels every directx and graphic related things
        */
        class GraphicModule : public DoremiEngine::Core::EngineModule
        {
            public:
            /**
            Returns a sub module manager which then gives access to other sub modules in the graphic module
            */
            virtual SubModuleManager& GetSubModuleManager() = 0;

            /**
               Doesnt do anything really...
            */
            virtual void Update() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Graphic::GraphicModule* (*CREATE_GRAPHIC_MODULE)(const DoremiEngine::Core::SharedContext&);
GRAPHIC_DLL_EXPORT DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_context);
}