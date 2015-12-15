#include <CameraClasses/ThirdPersonCamera.hpp>
#include <PlayerHandler.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/CameraManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>

namespace Doremi
{
    namespace Core
    {
        ThirdPersonCamera::ThirdPersonCamera(DoremiEngine::Graphic::Camera* p_camera) : m_camera(p_camera)
        {
        }

        ThirdPersonCamera::~ThirdPersonCamera() {}

        void ThirdPersonCamera::Update(double p_dt)
        {
            using namespace DirectX;
            int playerID = PlayerHandler::GetInstance()->GetPlayerEntityID();

            TransformComponent* playerTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(playerID);
            XMFLOAT4 orientation = playerTransform->rotation;
            XMVECTOR quater = XMLoadFloat4(&orientation);
            XMFLOAT3 direction = XMFLOAT3(0, 0, 1);
            XMFLOAT3 up = XMFLOAT3(0, 1, 0);
            XMVECTOR vup = XMLoadFloat3(&up);
            XMVECTOR dir = XMLoadFloat3(&direction);
            XMMATRIX t = XMMatrixRotationQuaternion(quater);
            dir = XMVector3Transform(dir, t);
            // vup = XMVector3Transform(vup, t);
            XMVECTOR pos = XMLoadFloat3(&playerTransform->position);


            // mat = XMMatrixInverse(&XMMatrixDeterminant(mat), mat);
            XMFLOAT4X4 viewMat;

            XMFLOAT4 plane = XMFLOAT4(0, 1, 0, 0);
            XMFLOAT4 light = XMFLOAT4(0, 1, 0, 0);
            XMVECTOR vecPlane = XMLoadFloat4(&plane);
            XMVECTOR veclight = XMLoadFloat4(&light);
            XMMATRIX projMat = XMMatrixShadow(vecPlane, veclight);
            XMVECTOR camDir = XMVector3Transform(dir, projMat);
            camDir = XMVector3Normalize(camDir);
            XMVECTOR finalDir = (pos - camDir * 2.5f) + XMLoadFloat3(&XMFLOAT3(0, 1, 0)) * 1.0;
            XMMATRIX mat = XMMatrixTranspose(XMMatrixLookAtLH(finalDir, pos + vup, vup));
            XMStoreFloat4x4(&viewMat, mat);
            m_camera->SetViewMatrix(viewMat);
        }
    }
}