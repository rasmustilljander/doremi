#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Graphic/Include/ShaderTypeEnum.hpp>
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
            Draw Object with the translation matrix specified (p_meshID = 0 is a unit box)
            */
            virtual void Draw(const int& p_meshID, const int& p_textureID,const DirectX::XMFLOAT4X4& p_translationMatrix) = 0;

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