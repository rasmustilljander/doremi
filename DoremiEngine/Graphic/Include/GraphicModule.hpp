#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Graphic/Include/ShaderTypeEnum.hpp>

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
            Initializes the important graphic things like direct x
            */
            virtual void Startup() = 0;

            /**
            TODO docs
            */
            virtual void Shutdown() = 0;

            /**
            Load model from specified file. Returns modelID
            */
            virtual int LoadObject(const std::string& p_fileName, const std::string& p_materialFileName) = 0;
            
            /**
            Load shader from specified file. Returns shaderID
            */
            virtual int LoadShader(ShaderType p_shaderType, const std::string& p_fileName) = 0;

            /**
            Set specified shader to active 
            */
            virtual void BindShader(ShaderType p_shaderType, int p_shaderID) = 0;
           
            /**
            Initializes DirectX
            */
            virtual void InitializeDirectX() = 0;

            /**
            Draw objects
            */
            virtual void Draw() = 0;

            /**
            Computes after effects such as glow and lighting
            */
            virtual void ComputeAfterEffects() = 0;

            /**
            Swaps buffers and clears backbuffer
            */
            virtual void EndDraw() = 0;
            // TODORK add more interface functions
           
        };
    }
}

extern "C" {
typedef DoremiEngine::Graphic::GraphicModule* (*CREATE_GRAPHIC_MODULE)(const DoremiEngine::Core::SharedContext&);
GRAPHIC_DLL_EXPORT DoremiEngine::Graphic::GraphicModule*
CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_context);
}