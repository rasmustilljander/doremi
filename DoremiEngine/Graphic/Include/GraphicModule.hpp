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
        TODO docs
        */
        class GraphicModule : public DoremiEngine::Core::EngineModule
        {
            public:
            virtual SubModuleManager& GetSubModuleManager() = 0;
            /**
               TODOKO docs
            */
            virtual void Update() = 0;


            // TODORK add more interface functions
        };
    }
}

extern "C" {
typedef DoremiEngine::Graphic::GraphicModule* (*CREATE_GRAPHIC_MODULE)(const DoremiEngine::Core::SharedContext&);
GRAPHIC_DLL_EXPORT DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_context);
}