#include <Internal/Manager/ComputeShaderManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <GraphicModuleImplementation.hpp>
#include <GraphicModuleContext.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        ComputeShaderManagerImpl::ComputeShaderManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_frustumArray = new FrustumArray();
        }


        ComputeShaderManagerImpl::~ComputeShaderManagerImpl() {}

        void ComputeShaderManagerImpl::SetUAV()
        {
            m_uav = NULL;
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();

            D3D11_BUFFER_DESC outputDesc;
            outputDesc.Usage = D3D11_USAGE_DEFAULT;
            outputDesc.ByteWidth = sizeof(FrustumInfo) * NUM_THREAD_BLOCKS;
            outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
            outputDesc.CPUAccessFlags = 0;
            outputDesc.StructureByteStride = sizeof(FrustumInfo);
            outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

            HRESULT hr = (m_directX.GetDevice()->CreateBuffer(&outputDesc, 0, &m_frustumArrayBuffer));

            outputDesc.Usage = D3D11_USAGE_STAGING;
            outputDesc.BindFlags = 0;
            outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            hr = (m_directX.GetDevice()->CreateBuffer(&outputDesc, 0, &m_frustumArrayBufferResult));

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.Flags = 0;
            uavDesc.Buffer.NumElements = NUM_THREAD_BLOCKS;
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

            hr = m_directX.GetDevice()->CreateUnorderedAccessView(m_frustumArrayBuffer, &uavDesc, &m_uav);
            m_directX.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_uav, 0);
        }

        ID3D11UnorderedAccessView* ComputeShaderManagerImpl::GetUAV() { return m_uav; }

        void ComputeShaderManagerImpl::CopyFrustumData()
        {

            ID3D11UnorderedAccessView* nullUAV[] = {NULL};

            ID3D11DeviceContext* m_context = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDeviceContext();

            m_context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

            m_context->CopyResource(m_frustumArrayBufferResult, m_frustumArrayBuffer);
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hr = m_context->Map(m_frustumArrayBufferResult, 0, D3D11_MAP_READ, 0, &mappedResource);

            if(SUCCEEDED(hr))
            {
                for(size_t i = 0; i < NUM_THREAD_BLOCKS; i++)
                {
                    m_frustumArray->frustum[i] = reinterpret_cast<FrustumInfo*>(mappedResource.pData)[i];
                }


                m_context->Unmap(m_frustumArrayBufferResult, 0);
            }
        }

        void ComputeShaderManagerImpl::UnmapBuffer()
        {
            ID3D11DeviceContext* m_context = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDeviceContext();
            m_context->Unmap(m_frustumArrayBufferResult, 0);
        }
    }
}