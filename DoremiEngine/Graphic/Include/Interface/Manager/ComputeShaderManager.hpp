#pragma once
#include <string>
#include <d3d11.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        enum BufferType;
        class ComputeShader;
        // struct ID3D11UnorderedAccessView;
        class ComputeShaderManager
        {
            public:
            // TODORK add functions
            virtual void CreateComputeShaders() = 0;
            virtual void SetFrustumUAV() = 0;
            virtual void SetUAV(BufferType index) = 0;
            virtual ID3D11UnorderedAccessView* GetUAV(int i) = 0;
            virtual void DispatchFrustum() = 0;
            virtual void DispatchCulling() = 0;
            virtual void CopyFrustumData() = 0;
            virtual void CopyCullingData() = 0;
        };
    }
}