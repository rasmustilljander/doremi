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

        Light* LightManagerImpl::AddLight(int p_type, float p_intensity, DirectX::XMFLOAT3 p_color, float p_coneAngle, DirectX::XMFLOAT3 p_direction,
                                          float p_penumAngle, DirectX::XMFLOAT3 p_position)
        {
            Light newLight;
            newLight.attenuation = DirectX::XMFLOAT3(1, 1, 1);
            newLight.color = p_color;
            newLight.coneAngle = p_coneAngle;
            newLight.direction = p_direction;
            newLight.enabled = 1;
            newLight.intensity = p_intensity;
            newLight.penumAgle = p_penumAngle;
            newLight.position = p_position;
            newLight.type = p_type;

            m_lightBuffer.lightList[m_lightcount] = newLight;
            m_lightcount++;
            return &newLight;
        }

        void LightManagerImpl::UpdateLights()
        {
            //TODORK Only update updated data
            D3D11_MAPPED_SUBRESOURCE tMS;
            m_deviceContext->Map(m_lBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &m_lightBuffer.lightList, sizeof(m_lightBuffer.lightList));
            m_deviceContext->Unmap(m_lBuffer, NULL);
            m_deviceContext->PSSetConstantBuffers(0, 1, &m_lBuffer);
            m_deviceContext->CSSetConstantBuffers(1, 1, &m_lBuffer);
        }
    }
}