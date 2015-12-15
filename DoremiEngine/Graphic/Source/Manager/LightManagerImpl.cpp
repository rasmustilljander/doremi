#include <Internal/Manager/LightManagerImpl.hpp>
#include <Internal/Light/LightInfoImpl.hpp>
#include <d3d11_1.h>


namespace DoremiEngine
{
    namespace Graphic
    {
        LightManagerImpl::LightManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}


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
            // m_device->CreateBuffer(&lightBufferDesc, NULL, &lBuffer);
            // for (int i = 0; i < 1000; i++)
            //{
            //    float x = (float)(rand() % 100) / 100;
            //    float y = (float)(rand() % 100) / 100;
            //    float z = (float)(rand() % 100) / 100;
            //    m_lightList[i].lightColor = DirectX::XMFLOAT3(x, y, z);
            //    //lBufferInfo.light[i].lightPosition = DirectX::XMFLOAT3(0, 0, 2);
            //    //lBufferInfo.light[i].lightColor = DirectX::XMFLOAT3(214.0f/255.0f, 174.0f/255.0f, 81.0f/255.0f);
            //    m_lightList[i].lightPosition = DirectX::XMFLOAT3((x * 10) - 5, (y * 14) - 5, (z * 7) - 2);
            //    m_lightList[i].cameraPos = DirectX::XMFLOAT3(0, 0, 0);
            //    m_lightList[i].constantAttenuation = 1;
            //    m_lightList[i].linearAttenuation = 1;
            //    m_lightList[i].quadricAttenuation = 1;
            //    lBufferInfo.light[i].shininess = 1;
            //    lBufferInfo.light[i].strength = 1;
            //}

            // D3D11_MAPPED_SUBRESOURCE tMS;
            // m_deviceContext->Map(lBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            // memcpy(tMS.pData, &lBufferInfo, sizeof(lBufferInfo));
            // m_deviceContext->Unmap(lBuffer, NULL);
            // m_deviceContext->PSSetConstantBuffers(0, 1, &lBuffer);
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