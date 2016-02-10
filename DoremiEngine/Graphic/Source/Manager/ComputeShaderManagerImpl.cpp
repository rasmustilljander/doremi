#include <Internal/Manager/ComputeShaderManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <Internal/Manager/LightManagerImpl.hpp>
#include <GraphicModuleImplementation.hpp>
#include <GraphicModuleContext.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Graphic
    {
        ComputeShaderManagerImpl::ComputeShaderManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_frustumArray = new FrustumArray();
            m_oLightCounter = new LightCounterBuffer();
            m_tLightCounter = new LightCounterBuffer();
            m_oLightIndexList = new LightIndexListBuffer();
            m_tLightIndexList = new LightIndexListBuffer();
            m_oLightGrid = new LightGridBuffer();
            m_tLightGrid = new LightGridBuffer();
            m_deviceContext = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDeviceContext();
            DirectX::XMFLOAT2 screenRes = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetScreenResolution();
            // m_oLightIndexList->lightIndexList.resize(screenRes.x / 16 * screenRes.y / 16 * 200); //Average of 200 lights per tile, TODORK change to
            // more appropriate value later
            // m_tLightIndexList->lightIndexList.resize(screenRes.x / 16 * screenRes.y / 16 * 200);

            CreateComputeShaders();
            SetUAV(BufferType::FRUSTUM);
            SetUAV(BufferType::O_LIGHTCOUNTER);
            SetUAV(BufferType::T_LIGHTCOUNTER);
            SetUAV(BufferType::O_LIGHTINDEXLIST);
            SetUAV(BufferType::T_LIGHTINDEXLIST);
            SetUAV(BufferType::O_LIGHTGRID);
            SetUAV(BufferType::T_LIGHTGRID);
        }

        void ComputeShaderManagerImpl::CreateComputeShaders()
        {
            ShaderManager& shaderManager = m_graphicContext.m_graphicModule->GetSubModuleManager().GetShaderManager();
            m_frustumShader = shaderManager.BuildComputeShader("FrustumComputeShader.hlsl");
            m_cullingShader = shaderManager.BuildComputeShader("LightCullingComputeShader.hlsl");
            m_blurHoriShader = shaderManager.BuildComputeShader("BlurHoriComputeShader.hlsl");
            m_blurVertShader = shaderManager.BuildComputeShader("BlurVertComputeShader.hlsl");
        }

        ComputeShaderManagerImpl::~ComputeShaderManagerImpl() {}

        void ComputeShaderManagerImpl::SetUAV(BufferType index)
        {
            unsigned int t_size;
            unsigned int t_stride;
            unsigned int t_numElements;
            switch(index)
            {
                case BufferType::FRUSTUM:
                    t_size = sizeof(FrustumInfo) * NUM_THREAD_BLOCKS;
                    t_stride = sizeof(FrustumInfo);
                    t_numElements = NUM_THREAD_BLOCKS;
                    break;
                case BufferType::O_LIGHTCOUNTER:
                    t_size = sizeof(unsigned int);
                    t_stride = t_size;
                    t_numElements = 1;
                    break;
                case BufferType::T_LIGHTCOUNTER:
                    t_size = sizeof(unsigned int);
                    t_stride = t_size;
                    t_numElements = 1;
                    break;
                case BufferType::O_LIGHTINDEXLIST:
                    t_size = sizeof(unsigned int) * NUM_THREAD_BLOCKS * 200;
                    t_stride = sizeof(unsigned int);
                    t_numElements = NUM_THREAD_BLOCKS * 200;
                    break;
                case BufferType::T_LIGHTINDEXLIST:
                    t_size = sizeof(unsigned int) * NUM_THREAD_BLOCKS * 200;
                    t_stride = sizeof(unsigned int);
                    t_numElements = NUM_THREAD_BLOCKS * 200;
                    break;
                case BufferType::O_LIGHTGRID:
                    t_size = sizeof(LightGridInfo) * NUM_THREAD_BLOCKS;
                    t_stride = sizeof(LightGridInfo);
                    t_numElements = NUM_THREAD_BLOCKS;
                    break;
                case BufferType::T_LIGHTGRID:
                    t_size = sizeof(LightGridInfo) * NUM_THREAD_BLOCKS;
                    t_stride = sizeof(LightGridInfo);
                    t_numElements = NUM_THREAD_BLOCKS;
                    break;
            }

            m_uav[index] = NULL;
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            D3D11_BUFFER_DESC outputDesc;
            outputDesc.Usage = D3D11_USAGE_DEFAULT;
            outputDesc.ByteWidth = t_size;
            outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            outputDesc.CPUAccessFlags = 0;
            outputDesc.StructureByteStride = t_stride;
            outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

            HRESULT hr = (m_directX.GetDevice()->CreateBuffer(&outputDesc, 0, &m_buffer[index]));

            outputDesc.Usage = D3D11_USAGE_STAGING;
            outputDesc.BindFlags = 0;
            outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            hr = (m_directX.GetDevice()->CreateBuffer(&outputDesc, 0, &m_bufferResult[index]));

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.Flags = 0;
            uavDesc.Buffer.NumElements = t_numElements;
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

            D3D11_SHADER_RESOURCE_VIEW_DESC t_SrvDesc;
            t_SrvDesc.Format = DXGI_FORMAT_UNKNOWN;
            t_SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            t_SrvDesc.BufferEx.FirstElement = 0;
            t_SrvDesc.BufferEx.Flags = 0;
            t_SrvDesc.BufferEx.NumElements = t_numElements;

            hr = m_directX.GetDevice()->CreateShaderResourceView(m_buffer[index], &t_SrvDesc, &m_srv[index]);

            if(FAILED(hr))
            {
                // ERROR MESSAGE
                std::cout << "Failed to create shader resource view" << std::endl;
            }

            hr = m_directX.GetDevice()->CreateUnorderedAccessView(m_buffer[index], &uavDesc, &m_uav[index]);
        }

        void ComputeShaderManagerImpl::SetSRV() {}

        ID3D11UnorderedAccessView* ComputeShaderManagerImpl::GetUAV(int i) { return m_uav[i]; }

        void ComputeShaderManagerImpl::DispatchFrustum()
        {
            // dispatch frustum compute shader
            ShaderManager& shaderManager = m_graphicContext.m_graphicModule->GetSubModuleManager().GetShaderManager();
            shaderManager.SetActiveComputeShader(m_frustumShader);

            ID3D11UnorderedAccessView* nullUAV[] = {NULL};

            // Set UAV and SRV
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uav[BufferType::FRUSTUM], 0);

            m_deviceContext->Dispatch(50, 50, 1);

            // Clear UAV and SRV
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


            // CopyFrustumData();
            CopyData(BufferType::FRUSTUM);
        }

        void ComputeShaderManagerImpl::DispatchCulling()
        {
            // dispatch culling compute shader
            ShaderManager& shaderManager = m_graphicContext.m_graphicModule->GetSubModuleManager().GetShaderManager();
            shaderManager.SetActiveComputeShader(m_cullingShader);
            ID3D11UnorderedAccessView* nullUAV[] = {NULL};
            ID3D11ShaderResourceView* nullSRV[] = {NULL};

            // Clear Pixel shader SRV
            m_deviceContext->PSSetShaderResources(1, 1, nullSRV);
            m_deviceContext->PSSetShaderResources(2, 1, nullSRV);
            m_deviceContext->PSSetShaderResources(3, 1, nullSRV);
            m_deviceContext->PSSetShaderResources(4, 1, nullSRV);

            // Set UAV and SRV
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_uav[BufferType::O_LIGHTCOUNTER], 0);
            m_deviceContext->CSSetUnorderedAccessViews(1, 1, &m_uav[BufferType::T_LIGHTCOUNTER], 0);
            m_deviceContext->CSSetUnorderedAccessViews(2, 1, &m_uav[BufferType::O_LIGHTINDEXLIST], 0);
            m_deviceContext->CSSetUnorderedAccessViews(3, 1, &m_uav[BufferType::T_LIGHTINDEXLIST], 0);
            m_deviceContext->CSSetUnorderedAccessViews(4, 1, &m_uav[BufferType::O_LIGHTGRID], 0);
            m_deviceContext->CSSetUnorderedAccessViews(5, 1, &m_uav[BufferType::T_LIGHTGRID], 0);
            m_deviceContext->CSSetShaderResources(0, 1, &m_srv[BufferType::FRUSTUM]);


            m_deviceContext->Dispatch(50, 50, 1);

            // Clear UAV and SRV
            m_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
            m_deviceContext->CSSetUnorderedAccessViews(1, 1, nullUAV, 0);
            m_deviceContext->CSSetUnorderedAccessViews(2, 1, nullUAV, 0);
            m_deviceContext->CSSetUnorderedAccessViews(3, 1, nullUAV, 0);
            m_deviceContext->CSSetUnorderedAccessViews(4, 1, nullUAV, 0);
            m_deviceContext->CSSetUnorderedAccessViews(5, 1, nullUAV, 0);
            m_deviceContext->CSSetShaderResources(0, 1, nullSRV);

            // Set SRV till Pixel shader
            m_deviceContext->PSSetShaderResources(1, 1, &m_srv[BufferType::O_LIGHTINDEXLIST]);
            m_deviceContext->PSSetShaderResources(2, 1, &m_srv[BufferType::T_LIGHTINDEXLIST]);
            m_deviceContext->PSSetShaderResources(3, 1, &m_srv[BufferType::O_LIGHTGRID]);
            m_deviceContext->PSSetShaderResources(4, 1, &m_srv[BufferType::T_LIGHTGRID]);

            CopyCullingData();
        }

        void ComputeShaderManagerImpl::DispatchBlurHorizontal()
        {
            ShaderManager& shaderManager = m_graphicContext.m_graphicModule->GetSubModuleManager().GetShaderManager();
            shaderManager.SetActiveComputeShader(m_blurHoriShader);

            int numGroupsX = ceil(1920.f / 256.f); // TODOCONFIG take width from config
            m_deviceContext->Dispatch(numGroupsX, 1080, 1);
        }

        void ComputeShaderManagerImpl::DispatchBlurVertical()
        {
            ShaderManager& shaderManager = m_graphicContext.m_graphicModule->GetSubModuleManager().GetShaderManager();
            shaderManager.SetActiveComputeShader(m_blurVertShader);

            int numGroupsY = ceil(1080.f / 256.f); // TODOCONFIG take height from config
            m_deviceContext->Dispatch(1920, numGroupsY, 1);
        }

        void ComputeShaderManagerImpl::CopyCullingData()
        {
            CopyData(BufferType::O_LIGHTCOUNTER);
            CopyData(BufferType::T_LIGHTCOUNTER);
            CopyData(BufferType::O_LIGHTINDEXLIST);
            CopyData(BufferType::T_LIGHTINDEXLIST);
            CopyData(BufferType::O_LIGHTGRID);
            CopyData(BufferType::T_LIGHTGRID);
        }

        void ComputeShaderManagerImpl::CopyData(BufferType index)
        {
            ID3D11UnorderedAccessView* nullUAV[] = {NULL};

            ID3D11DeviceContext* m_context = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDeviceContext();


            m_context->CopyResource(m_bufferResult[index], m_buffer[index]);
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hr = m_context->Map(m_bufferResult[index], 0, D3D11_MAP_READ, 0, &mappedResource);

            if(SUCCEEDED(hr))
            {
                switch(index)
                {
                    case 0:
                        for(size_t i = 0; i < NUM_THREAD_BLOCKS; i++)
                        {
                            m_frustumArray->frustum[i] = reinterpret_cast<FrustumInfo*>(mappedResource.pData)[i];
                        }
                        break;
                    case 1:
                        m_oLightCounter->counter = reinterpret_cast<unsigned int>(mappedResource.pData);
                        break;
                    case 2:
                        m_tLightCounter->counter = reinterpret_cast<unsigned int>(mappedResource.pData);
                        break;
                    case 3:
                        for(size_t i = 0; i < NUM_THREAD_BLOCKS * 200; i++)
                        {
                            m_oLightIndexList->LightIndexList[i] = reinterpret_cast<unsigned int*>(mappedResource.pData)[i];
                        }
                        break;
                    case 4:
                        for(size_t i = 0; i < NUM_THREAD_BLOCKS * 200; i++)
                        {
                            m_tLightIndexList->LightIndexList[i] = reinterpret_cast<unsigned int*>(mappedResource.pData)[i];
                        }
                        break;
                    case 5:
                        for(size_t i = 0; i < NUM_THREAD_BLOCKS; i++)
                        {
                            m_oLightGrid->lightGridInfo[i] = reinterpret_cast<LightGridInfo*>(mappedResource.pData)[i];
                        }
                        break;
                    case 6:
                        for(size_t i = 0; i < NUM_THREAD_BLOCKS; i++)
                        {
                            m_tLightGrid->lightGridInfo[i] = reinterpret_cast<LightGridInfo*>(mappedResource.pData)[i];
                        }
                        break;
                }

                m_context->Unmap(m_bufferResult[index], 0);
            }
        }
    }
}