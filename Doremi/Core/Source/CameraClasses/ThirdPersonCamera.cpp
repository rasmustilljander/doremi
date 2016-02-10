#include <CameraClasses/ThirdPersonCamera.hpp>
#include <PlayerHandler.hpp>
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
        ThirdPersonCamera::ThirdPersonCamera(DoremiEngine::Graphic::Camera* p_camera, const float& p_distanceFromPlayer, const float& p_minAngle, const float& p_maxAngle)
            : m_camera(p_camera), m_distanceFromPlayer(p_distanceFromPlayer), m_maxAngle(p_maxAngle), m_minAngle(p_minAngle)
        {
            m_angle = (p_minAngle + p_maxAngle) * 0.5f;
        }

        ThirdPersonCamera::~ThirdPersonCamera() {}

        void ThirdPersonCamera::Update()
        {
            using namespace DirectX;

            EntityID playerID = 0;
            if(!PlayerHandler::GetInstance()->GetDefaultPlayerEntityID(playerID))
            {
                return;
            }
            // m_angle = 0;
            TransformComponent* playerTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(playerID);
            XMFLOAT4 orientation = playerTransform->rotation;
            XMVECTOR position = XMLoadFloat3(&playerTransform->position);
            XMVECTOR quater = XMLoadFloat4(&orientation);
            XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0)); // The upvector of the character should always be this
            XMVECTOR forward = XMLoadFloat3(&XMFLOAT3(0, 0, 1)); // Standard forward vector
            forward = XMVector3Rotate(forward, quater); // Rotate forward vector with player orientation TODOKO Should maybe disregard some rotations?
            forward = XMVector3Normalize(forward);
            XMVECTOR right = XMVector3Cross(forward, up);
            XMVECTOR specialRot = XMQuaternionRotationAxis(right, m_angle);
            //XMQuaternionMultiply(quater, specialRot) is total rotation in one quternion
            forward = XMVector3Rotate(forward, specialRot); // Rotate forward vector with angle around local x vector 
            forward = XMVector3Normalize(forward);
            XMVECTOR cameraPosition = position - forward * m_distanceFromPlayer;
            // float offsetY = 5 * cosf(m_angle); // Get offset in Y
            // cameraPosition += XMLoadFloat3(&XMFLOAT3(0, offsetY, 0)); // Set offset in Y

            XMMATRIX worldMatrix =
                XMMatrixTranspose(XMMatrixLookAtLH(cameraPosition, position, up)); // vup is added to position so you look abit above the player
            // XMVECTOR forwardQuater = XMQuaternionRotationNormal(forward,0);
            XMFLOAT4X4 viewMat;
            XMStoreFloat4x4(&viewMat, worldMatrix);
            m_camera->SetViewMatrix(viewMat);
            XMFLOAT3 t_camPos;
            XMStoreFloat3(&t_camPos, cameraPosition);

            m_camera->SetCameraPosition(t_camPos);
        }

        void ThirdPersonCamera::UpdateInput(const double& p_dt)
        {
            InputHandlerClient* inputHandler = (InputHandlerClient*)PlayerHandler::GetInstance()->GetDefaultInputHandler();
            float wantedAngle = m_angle; // the angle we want to reach
            wantedAngle += inputHandler->GetMouseMovementY() * 0.001;
            if(wantedAngle < m_maxAngle && wantedAngle > m_minAngle)
            {
                m_angle = wantedAngle;
            }
        }
    }
}