#include <Internal/Manager/LightManagerImpl.hpp>
#include <Internal/Light/LightInfoImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <d3d11_1.h>


namespace DoremiEngine
{
    namespace Graphic
    {
        LightManagerImpl::LightManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_device = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDevice();
            m_deviceContext = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDeviceContext();
        }


        LightManagerImpl::~LightManagerImpl() {}

        void LightManagerImpl::InitLightManager()
        {
            D3D11_BUFFER_DESC lightBufferDesc;
            ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
            lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            lightBufferDesc.ByteWidth = sizeof(m_lightList);
            lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            lightBufferDesc.MiscFlags = 0;
            lightBufferDesc.StructureByteStride = 0;
            ID3D11Buffer* lBuffer;
            m_device->CreateBuffer(&lightBufferDesc, NULL, &lBuffer);

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(lBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &lBufferInfo, sizeof(lBufferInfo));
            m_deviceContext->Unmap(lBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(0, 1, &lBuffer);
        }

        int LightManagerImpl::AddLight(LightInfo light)
        {
            m_lightList.push_back(light);
            return m_lightList.size() - 1;
        }

        LightInfo LightManagerImpl::GetLight(int index) { return m_lightList[index]; }

        std::vector<LightInfo> LightManagerImpl::GetLightList() { return m_lightList; }
    }
}