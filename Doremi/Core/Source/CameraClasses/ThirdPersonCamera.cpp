#include <CameraClasses/ThirdPersonCamera.hpp>
#include <PlayerHandlerClient.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <InputHandlerClient.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/CameraManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>

namespace Doremi
{
    namespace Core
    {
        ThirdPersonCamera::ThirdPersonCamera(DoremiEngine::Graphic::Camera* p_camera, const float& p_distanceFromPlayer)
            : m_camera(p_camera), m_distanceFromPlayer(p_distanceFromPlayer)
        {
            m_acctualPosition = m_camera->GetCameraPosition();
            m_acctualFocus = m_acctualPosition;
            m_interpolationSpeed = 0.3; // TODOCONFIG
        }

        ThirdPersonCamera::~ThirdPersonCamera() {}

        void ThirdPersonCamera::Update()
        {
            using namespace DirectX;

            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            // If player exists
            if(!t_playerHandler->PlayerExists())
            {
                return;
            }

            EntityID playerID = t_playerHandler->GetPlayerEntityID();
            // m_angle = 0;
            TransformComponent* playerTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(playerID);
            XMFLOAT4 orientation = playerTransform->rotation;
            XMVECTOR position = XMLoadFloat3(&playerTransform->position);
            XMVECTOR quater = XMLoadFloat4(&orientation);

            // the standar up which will be altered by the quaternion
            XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
            XMVECTOR realUp = XMVector3Rotate(up, quater);

            XMVECTOR forward = XMLoadFloat3(&XMFLOAT3(0, 0, 1)); // Standard forward vector
            forward = XMVector3Rotate(forward, quater); // Rotate forward vector with player orientation TODOKO Should maybe disregard some rotations?
            forward = XMVector3Normalize(forward);

            XMVECTOR cameraPosition = position - forward * m_distanceFromPlayer;
            realUp = XMVector3Normalize(realUp);


            XMVECTOR focus = position + realUp * 4;
            cameraPosition += realUp * 4; // Set offset in Y TODOCONFIG

            cameraPosition = XMVectorLerp(XMLoadFloat3(&m_acctualPosition), cameraPosition, m_interpolationSpeed);
            focus = XMVectorLerp(XMLoadFloat3(&m_acctualFocus), focus, m_interpolationSpeed);

            XMVECTOR cameraDirection = XMVector3Normalize(cameraPosition - focus);
            cameraPosition = focus + cameraDirection * m_distanceFromPlayer;


            XMMATRIX wantedMatrix = XMMatrixTranspose(XMMatrixLookAtLH(cameraPosition, focus, up));
            XMVECTOR cameraWantedRotation = XMQuaternionRotationMatrix(wantedMatrix);

            XMFLOAT4X4 viewMat;
            XMStoreFloat4x4(&viewMat, wantedMatrix);
            m_camera->SetViewMatrix(viewMat);
            XMFLOAT3 t_camPos;
            XMStoreFloat3(&t_camPos, cameraPosition);

            m_camera->SetCameraPosition(t_camPos);
            XMStoreFloat3(&m_acctualPosition, cameraPosition);
            XMStoreFloat3(&m_acctualFocus, focus);
        }

        void ThirdPersonCamera::UpdateInput(const double& p_dt) {}
    }
}