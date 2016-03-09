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
            m_previousPosition = m_camera->GetCameraPosition();
            m_previousFocus = m_previousPosition;
            m_nextPosition = m_previousPosition;
            m_nextFocus = m_previousPosition;
            m_interpolationSpeed = 0.3; // TODOCONFIG
        }

        ThirdPersonCamera::~ThirdPersonCamera() {}

        void ThirdPersonCamera::UpdatePositions()
        {
            using namespace DirectX;
            // Update previous positions
            m_previousPosition = m_nextPosition;
            m_previousFocus = m_nextFocus;

            // Update next positions

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

            XMVECTOR nextPosition = XMLoadFloat3(&m_nextPosition);
            XMVECTOR nextFocus = XMLoadFloat3(&m_nextFocus);

            nextPosition = XMVectorLerp(nextPosition, cameraPosition, m_interpolationSpeed);
            nextFocus = XMVectorLerp(nextFocus, focus, m_interpolationSpeed);

            XMStoreFloat3(&m_nextFocus, nextFocus);
            XMStoreFloat3(&m_nextPosition, nextPosition);
        }

        void ThirdPersonCamera::UpdateInterpolation(const double& p_alpha)
        {
            using namespace DirectX;
            XMVECTOR cameraAcctualPosition = XMVectorLerp(XMLoadFloat3(&m_previousPosition), XMLoadFloat3(&m_nextPosition), p_alpha);
            XMVECTOR cameraAcctualfocus = XMVectorLerp(XMLoadFloat3(&m_previousFocus), XMLoadFloat3(&m_nextFocus), p_alpha);

            XMVECTOR cameraDirection = XMVector3Normalize(cameraAcctualPosition - cameraAcctualfocus);
            cameraAcctualPosition = cameraAcctualfocus + cameraDirection * m_distanceFromPlayer;

            XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
            XMMATRIX wantedMatrix = XMMatrixTranspose(XMMatrixLookAtLH(cameraAcctualPosition, cameraAcctualfocus, up));

            XMFLOAT4X4 viewMat;
            XMStoreFloat4x4(&viewMat, wantedMatrix);
            m_camera->SetViewMatrix(viewMat);

            XMFLOAT3 t_camPos;
            XMStoreFloat3(&t_camPos, cameraAcctualPosition);
            m_camera->SetCameraPosition(t_camPos);
        }
    }
}