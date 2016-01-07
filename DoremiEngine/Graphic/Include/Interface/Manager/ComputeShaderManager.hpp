#pragma once
#include <string>
#include <d3d11.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        class ComputeShader;
        // struct ID3D11UnorderedAccessView;
        class ComputeShaderManager
        {
            public:
            // TODORK add functions
            virtual void SetUAV() = 0;
            virtual ID3D11UnorderedAccessView* GetUAV() = 0;
            virtual void CopyFrustumData() = 0;
            virtual void UnmapBuffer() = 0;
        };
    }
}