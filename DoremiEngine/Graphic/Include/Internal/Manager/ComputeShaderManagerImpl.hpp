#pragma once

#define NUM_THREAD_BLOCKS 2500
#define NUM_THREADS 640000

#include <Interface/Manager/ComputeShaderManager.hpp>
#include <Internal/Light/LightInfoImpl.hpp>
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
namespace DoremiEngine
{
    namespace Graphic
    {
        struct LightInfo;
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

        struct LightIndexListBuffer
        {
            // std::vector<unsigned int> lightIndexList;
            unsigned int LightIndexList[NUM_THREAD_BLOCKS * 200];
        };

        struct LightGridInfo
        {
            unsigned int offset;
            unsigned int value;
        };

        struct LightGridBuffer
        {
            LightGridInfo lightGridInfo[NUM_THREAD_BLOCKS];
        };

        struct LightCounterBuffer
        {
            float counter;
        };

        struct FrustumArray
        {
            FrustumInfo frustum[NUM_THREAD_BLOCKS];
        };


        enum BufferType
        {
            FRUSTUM,
            O_LIGHTCOUNTER,
            T_LIGHTCOUNTER,
            O_LIGHTINDEXLIST,
            T_LIGHTINDEXLIST,
            O_LIGHTGRID,
            T_LIGHTGRID,

            NUM_BUFFERS
        };


        class ComputeShaderManagerImpl : public ComputeShaderManager
        {
            public:
            ComputeShaderManagerImpl(const GraphicModuleContext& p_graphicContext);
            ~ComputeShaderManagerImpl();
            void CreateComputeShaders() override;
            // Set UAV for compute shaders. Index specifies which struct to send
            void SetUAV(BufferType index) override;
            void SetSRV();
            ID3D11UnorderedAccessView* GetUAV(int i) override;
            void DispatchFrustum() override;
            void DispatchCulling() override;
            void CopyCullingData() override;
            void CopyData(BufferType index);

            private:
            const GraphicModuleContext& m_graphicContext;
            ID3D11DeviceContext* m_deviceContext;
            FrustumArray* m_frustumArray;
            LightCounterBuffer* m_oLightCounter;
            LightCounterBuffer* m_tLightCounter;
            LightIndexListBuffer* m_oLightIndexList;
            LightIndexListBuffer* m_tLightIndexList;
            LightGridBuffer* m_oLightGrid;
            LightGridBuffer* m_tLightGrid;
            ID3D11UnorderedAccessView* m_uav[NUM_BUFFERS];
            ID3D11ShaderResourceView* m_srv[NUM_BUFFERS];
            ID3D11Buffer* m_buffer[NUM_BUFFERS];
            ID3D11Buffer* m_bufferResult[NUM_BUFFERS];
            ComputeShader* m_frustumShader;
            ComputeShader* m_cullingShader;
        };
    }
}
