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
            //////////TODORK move to other place/////////////
            Light light = Light();
            light.attenuation = DirectX::XMFLOAT3(2, 2, 2);
            light.color = DirectX::XMFLOAT3(0.5, 0.2, 0.7);
            light.coneAngle = 0;
            light.direction = DirectX::XMFLOAT3(1, 1, 1);
            light.intensity = 2;
            light.penumAgle = 0;
            light.position = DirectX::XMFLOAT3(1.5, -10, 6);
            light.type = 3;

            Light light2 = Light();
            light2.attenuation = DirectX::XMFLOAT3(2, 2, 2);
            light2.color = DirectX::XMFLOAT3(0.2, 0.3, 0.9);
            light2.coneAngle = 0;
            light2.direction = DirectX::XMFLOAT3(1, 1, 1);
            light2.intensity = 2;
            light2.penumAgle = 0;
            light2.position = DirectX::XMFLOAT3(-1.5, -10, 4);
            light2.type = 3;

            Light light3 = Light();
            light3.attenuation = DirectX::XMFLOAT3(2, 2, 2);
            light3.color = DirectX::XMFLOAT3(0.2, 0.7, 0.1);
            light3.coneAngle = 0;
            light3.direction = DirectX::XMFLOAT3(1, 1, 1);
            light3.intensity = 2;
            light3.penumAgle = 0;
            light3.position = DirectX::XMFLOAT3(-1.5, -10, 6);
            light3.type = 3;

            Light light4 = Light();
            light4.attenuation = DirectX::XMFLOAT3(2, 2, 2);
            light4.color = DirectX::XMFLOAT3(0.7, 0.7, 0.2);
            light4.coneAngle = 0;
            light4.direction = DirectX::XMFLOAT3(1, 1, 1);
            light4.intensity = 2;
            light4.penumAgle = 0;
            light4.position = DirectX::XMFLOAT3(1.5, -10, 4);
            light4.type = 3;


            m_lightBuffer.lightList[0] = light;
            m_lightBuffer.lightList[1] = light2;
            m_lightBuffer.lightList[2] = light3;
            m_lightBuffer.lightList[3] = light4;
            ////////////////////////////////////////////////

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
            int index = sizeof(m_lightBuffer.lightList) / sizeof(Light);
            m_lightBuffer.lightList[index] = light;
            return index;
        }

        Light LightManagerImpl::GetLight(int index) { return m_lightBuffer.lightList[index]; }
    }
}