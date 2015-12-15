#pragma once
#include <Internal/Manager/CameraManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Camera/CameraImpl.hpp>

#include <DirectXMath.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <string>
#include <d3d11_1.h>
#include <d3dcompiler.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        CameraManagerImpl::CameraManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}

        CameraManagerImpl::~CameraManagerImpl() {}

        void CameraManagerImpl::Initialize()
        {
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = sizeof(CameraMatrices);
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0;
            bd.StructureByteStride = 0;
            m_directX.GetDevice()->CreateBuffer(&bd, NULL, &m_cameraBuffer);
            using namespace DirectX;
        }

        Camera* CameraManagerImpl::BuildNewCamera(DirectX::XMFLOAT4X4& p_projectionMatrix)
        {
            using namespace DirectX;
            Camera* newCamera = new CameraImpl();
            newCamera->SetProjectionMatrix(p_projectionMatrix);
            XMFLOAT4X4 viewMatrix;
            XMFLOAT3 position = XMFLOAT3(0, 0, 0);
            XMFLOAT3 direction = XMFLOAT3(0, 0, 1);
            XMFLOAT3 up = XMFLOAT3(0, 1, 0);
            XMMATRIX matrix = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&direction), XMLoadFloat3(&up));
            XMStoreFloat4x4(&viewMatrix, matrix);
            newCamera->SetViewMatrix(viewMatrix);
            return newCamera;
        }

        void CameraManagerImpl::PushCameraToDevice(const Camera& p_camera)
        {
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            CameraMatrices bufferInfo = p_camera.GetCameraMatrices();
            D3D11_MAPPED_SUBRESOURCE tMS;
            m_directX.GetDeviceContext()->Map(m_cameraBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, &bufferInfo, sizeof(CameraMatrices));
            m_directX.GetDeviceContext()->Unmap(m_cameraBuffer, NULL);
            m_directX.GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_cameraBuffer);
        }
    }
}