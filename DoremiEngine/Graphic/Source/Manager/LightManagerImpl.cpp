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
            m_lightcount = 0;
            InitLightManager();
        }


        LightManagerImpl::~LightManagerImpl() {}

        void LightManagerImpl::InitLightManager()
        {
            //////////TODORK move to other place/////////////
            Light light = Light();
            light.attenuation = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
            light.color = DirectX::XMFLOAT3(0.5f, 0.2f, 0.7f);
            light.coneAngle = 0.0f;
            light.direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
            light.intensity = 2.0f;
            light.penumAgle = 0.0f;
            light.position = DirectX::XMFLOAT3(10.5f, 1.f, 6.0f);
            light.enabled = 1;
            light.type = 3;

            Light light2 = Light();
            light2.attenuation = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
            light2.color = DirectX::XMFLOAT3(0.2f, 0.3f, 0.9f);
            light2.coneAngle = 0.0f;
            light2.direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
            light2.intensity = 2.0f;
            light2.penumAgle = 0.0f;
            light2.position = DirectX::XMFLOAT3(-1.5f, 10.f, 4.0f);
            light2.enabled = 1;
            light2.type = 3;

            Light light3 = Light();
            light3.attenuation = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
            light3.color = DirectX::XMFLOAT3(0.2f, 0.7f, 0.1f);
            light3.coneAngle = 0.0f;
            light3.direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
            light3.intensity = 2.0f;
            light3.penumAgle = 0.0f;
            light3.position = DirectX::XMFLOAT3(-1.5f, 10.f, 6.0f);
            light3.enabled = 1;
            light3.type = 3;

            Light light4 = Light();
            light4.attenuation = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
            light4.color = DirectX::XMFLOAT3(0.7f, 0.7f, 0.2f);
            light4.coneAngle = 0.0f;
            light4.direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
            light4.intensity = 2.0f;
            light4.penumAgle = 0.0f;
            light4.position = DirectX::XMFLOAT3(1.5f, 10.f, 4.0f);
            light4.enabled = 1;
            light4.type = 3;


            m_lightBuffer.lightList[0] = light;
            m_lightBuffer.lightList[1] = light4;
            m_lightBuffer.lightList[2] = light3;
            m_lightBuffer.lightList[3] = light2;

            for(int i = 4; i < 95; i++)
                m_lightBuffer.lightList[i] = m_lightBuffer.lightList[1];

            ////////////////////////////////////////////////

            D3D11_BUFFER_DESC lightBufferDesc;
            ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
            lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            lightBufferDesc.ByteWidth = sizeof(m_lightBuffer.lightList);
            lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            lightBufferDesc.MiscFlags = 0;
            lightBufferDesc.StructureByteStride = 0;
            m_device->CreateBuffer(&lightBufferDesc, NULL, &m_lBuffer);

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_lBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &m_lightBuffer.lightList, sizeof(m_lightBuffer.lightList));
            m_deviceContext->Unmap(m_lBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(0, 1, &m_lBuffer);
            m_deviceContext->CSSetConstantBuffers(1, 1, &m_lBuffer);
        }

        Light* LightManagerImpl::AddLight(int type, float intensity, DirectX::XMFLOAT3 color, float coneAngle, DirectX::XMFLOAT3 direction,
                                          float penumAngle, DirectX::XMFLOAT3 position)
        {
            Light newLight;
            newLight.attenuation = DirectX::XMFLOAT3(1, 1, 1);
            newLight.color = color;
            newLight.coneAngle = coneAngle;
            newLight.direction = direction;
            newLight.enabled = 1;
            newLight.intensity = intensity;
            newLight.penumAgle = penumAngle;
            newLight.position = position;
            newLight.type = type;

            m_lightBuffer.lightList[m_lightcount] = newLight;
            m_lightcount++;
            return &newLight;
        }

        void LightManagerImpl::TestFunc()
        {
            m_lightBuffer.lightList[0].position.x += 0.1;

            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_lBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &m_lightBuffer.lightList, sizeof(m_lightBuffer.lightList));
            m_deviceContext->Unmap(m_lBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(0, 1, &m_lBuffer);
            m_deviceContext->CSSetConstantBuffers(1, 1, &m_lBuffer);
        }
    }
}