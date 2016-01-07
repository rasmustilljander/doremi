#pragma once

#define NUM_THREAD_BLOCKS 2500
#define NUM_THREADS 640000

#include <Interface/Manager/ComputeShaderManager.hpp>
#include <DirectXMath.h>
#include <d3d11.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        struct Plane
        {
            DirectX::XMFLOAT3 normal; // plane normal
            float distance; // distance to origin
        };
        struct FrustumInfo
        {
            Plane plane[4];
        };

        struct FrustumArray
        {
            FrustumInfo frustum[NUM_THREAD_BLOCKS];
        };

        class ComputeShaderManagerImpl : public ComputeShaderManager
        {
            public:
            ComputeShaderManagerImpl(const GraphicModuleContext& p_graphicContext);
            ~ComputeShaderManagerImpl();
            void SetUAV() override;
            ID3D11UnorderedAccessView* GetUAV() override;
            void ComputeShaderManagerImpl::CopyFrustumData() override;
            void ComputeShaderManagerImpl::UnmapBuffer() override;


            private:
            const GraphicModuleContext& m_graphicContext;
            FrustumArray* m_frustumArray;
            FrustumInfo* m_frustumInfo;
            ID3D11UnorderedAccessView* m_uav;
            ID3D11Buffer* m_frustumArrayBuffer;
            ID3D11Buffer* m_frustumArrayBufferResult;
        };
    }
}
