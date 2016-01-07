#include <CameraClasses/FreeLookCamera.hpp>
#include <PlayerHandler.hpp>
#include <InputHandlerClient.hpp>

// Engine
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>

namespace Doremi
{
    namespace Core
    {
        FreeLookCamera::FreeLookCamera(DoremiEngine::Graphic::Camera* p_camera)
            : m_camera(p_camera),
              m_camUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)),
              m_camTarget(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)),
              m_camPos(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)),
              m_defaultForward(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)),
              m_defaultRight(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)),
              m_camForward(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)),
              m_camRight(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)),
              m_camPitch(0.0f),
              m_camYaw(0.0f),
              m_moveLeftRight(0.0f),
              m_moveForwardBackward(0.0f),
              m_speed(10)
        {
            using namespace DirectX;
        }

        FreeLookCamera::~FreeLookCamera() {}

        void FreeLookCamera::Update(double p_dt)
        {
            HandleUserCMD(p_dt);
            using namespace DirectX;
            XMStoreFloat4x4(&m_camRotationMatrix, XMMatrixRotationRollPitchYaw(m_camPitch, m_camYaw, 0));
            XMStoreFloat3(&m_camTarget, XMVector3TransformCoord(XMLoadFloat3(&m_defaultForward), XMLoadFloat4x4(&m_camRotationMatrix)));
            XMStoreFloat3(&m_camTarget, XMVector3Normalize(XMLoadFloat3(&m_camTarget)));

            XMStoreFloat3(&m_camRight, XMVector3TransformCoord(XMLoadFloat3(&m_defaultRight), XMLoadFloat4x4(&m_camRotationMatrix)));
            XMStoreFloat3(&m_camForward, XMVector3TransformCoord(XMLoadFloat3(&m_defaultForward), XMLoadFloat4x4(&m_camRotationMatrix)));
            XMStoreFloat3(&m_camUp, XMVector3Cross(XMLoadFloat3(&m_camForward), XMLoadFloat3(&m_camRight)));

            XMVECTOR tempCam;
            tempCam = XMLoadFloat3(&m_camPos);

            tempCam += m_moveLeftRight * XMLoadFloat3(&m_camRight);
            tempCam += m_moveForwardBackward * XMLoadFloat3(&m_camForward);

            XMStoreFloat3(&m_camPos, tempCam);

            m_moveLeftRight = 0.0f;
            m_moveForwardBackward = 0.0f;

            XMStoreFloat3(&m_camTarget, XMLoadFloat3(&m_camPos) + XMLoadFloat3(&m_camTarget));
            XMFLOAT4X4 viewMatrix;
            XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&m_camPos), XMLoadFloat3(&m_camTarget), XMLoadFloat3(&m_camUp))));
            XMStoreFloat3(&m_camRight, XMVector3Normalize(XMLoadFloat3(&m_camRight)));

            m_camera->SetViewMatrix(viewMatrix);
        }

        void FreeLookCamera::HandleUserCMD(double pDT)
        {
            InputHandlerClient* inputHandler = (InputHandlerClient*)PlayerHandler::GetInstance()->GetDefaultInputHandler();

            if (inputHandler != nullptr)
            {
                if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::DebugLeft))
                {
                    m_moveLeftRight -= m_speed * pDT;
                }
                if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::DebugRight))
                {
                    m_moveLeftRight += m_speed * pDT;
                }
                if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::DebugForward))
                {
                    m_moveForwardBackward += m_speed * pDT;
                }
                if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::DebugBackward))
                {
                    m_moveForwardBackward -= m_speed * pDT;
                }
                if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Fire))
                {
                    m_camYaw += inputHandler->GetMouseMovementX() * 0.001f;
                    m_camPitch += inputHandler->GetMouseMovementY() * 0.001f;
                }
            }
        }
    }
}