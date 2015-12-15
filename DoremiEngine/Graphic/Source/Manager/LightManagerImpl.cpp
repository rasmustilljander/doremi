#include <Internal/Manager/LightManagerImpl.hpp>
#include <Internal/Light/LightInfoImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>


namespace DoremiEngine
{
    namespace Graphic
    {
        LightManagerImpl::LightManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_device = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDevice();
            m_deviceContext = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDeviceContext();
            InitLightManager();
        }


        LightManagerImpl::~LightManagerImpl() {}

        void LightManagerImpl::InitLightManager()
        {
            Light light = Light();
            light.attenuation = DirectX::XMFLOAT3(1, 1, 1);
            light.color = DirectX::XMFLOAT3(0.5, 0.5, 1);
            light.coneAngle = 0;
            light.direction = DirectX::XMFLOAT3(1, 1, 1);
            light.intensity = 1;
            light.penumAgle = 0;
            light.position = DirectX::XMFLOAT3(0, 5, 0);
            light.type = 3;


            m_lightBuffer.lightList[0] = light;
            m_lightBuffer.lightList[1] = light;
            m_lightBuffer.lightList[2] = light;

            D3D11_BUFFER_DESC lightBufferDesc;
            ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
            lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            lightBufferDesc.ByteWidth = sizeof(m_lightBuffer.lightList);
            lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            lightBufferDesc.MiscFlags = 0;
            lightBufferDesc.StructureByteStride = 0;
            ID3D11Buffer* lBuffer;
            m_device->CreateBuffer(&lightBufferDesc, NULL, &lBuffer);

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(lBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &m_lightBuffer.lightList, sizeof(m_lightBuffer.lightList));
            m_deviceContext->Unmap(lBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(0, 1, &lBuffer);
        }

        int LightManagerImpl::AddLight(Light light)
        {
            // m_lightBuffer.lightList.push_back(light);
            // return m_lightBuffer.lightList.size() - 1;
            return 1;
        }

        Light LightManagerImpl::GetLight(int index) { return m_lightBuffer.lightList[index]; }

        // std::vector<LightInfoImpl> LightManagerImpl::GetLightList() { return m_lightBuffer.lightList; }
    }
}