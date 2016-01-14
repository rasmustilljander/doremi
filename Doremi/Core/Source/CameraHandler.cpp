#include <CameraHandler.hpp>
#include <InputHandler.hpp>
#include <CameraClasses/ThirdPersonCamera.hpp>
#include <CameraClasses/FreeLookCamera.hpp>
#include <PlayerHandler.hpp>
#include <InputHandlerClient.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/CameraManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
// Third party


namespace Doremi
{
    namespace Core
    {
        CameraHandler* CameraHandler::m_singleton = nullptr;

        void CameraHandler::StartCameraHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new CameraHandler(p_sharedContext);
            }
        }
        CameraHandler* CameraHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                // TODOKO error message, not started
            }
            return m_singleton;
        }

        CameraHandler::CameraHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            using namespace DirectX;
            DoremiEngine::Graphic::CameraManager& t_graphicModuleCameraManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetCameraManager();
            XMFLOAT4X4 projection;
            XMMATRIX mat =
                XMMatrixTranspose(XMMatrixPerspectiveFovLH(90.0f * 3.14f / 180.0f, 800.0f / 600.0f, 0.1f, 1000.0f)); // TODOKO use config values
            XMStoreFloat4x4(&projection, mat);

            DoremiEngine::Graphic::Camera* freecamera = t_graphicModuleCameraManager.BuildNewCamera(projection);
            DoremiEngine::Graphic::Camera* thirdPersonCamera = t_graphicModuleCameraManager.BuildNewCamera(projection);
            m_thirdPersonCamera = new ThirdPersonCamera(thirdPersonCamera);
            m_freeLookCamera = new FreeLookCamera(freecamera);
            m_currentCamera = CameraType::FREELOOK;
        }

        CameraHandler::~CameraHandler() {}

        void CameraHandler::Update(double p_dt)
        {
            DoremiEngine::Graphic::CameraManager& t_graphicModuleCameraManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetCameraManager();
            InputHandlerClient* inputHandler = (InputHandlerClient*)PlayerHandler::GetInstance()->GetDefaultInputHandler();
            if(inputHandler != nullptr)
            {
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::DebugButton))
                {
                    switch(m_currentCamera)
                    {
                        case CameraType::FREELOOK:
                            ChangeCamera(CameraType::THIRDPERSON);
                            break;
                        case CameraType::THIRDPERSON:
                            ChangeCamera(CameraType::FREELOOK);
                            break;
                        default:
                            break;
                    }
                }
                switch(m_currentCamera)
                {
                    case CameraType::FREELOOK:
                        inputHandler->SetCursorInvisibleAndMiddle(false);
                        m_freeLookCamera->Update(p_dt);
                        t_graphicModuleCameraManager.PushCameraToDevice(m_freeLookCamera->GetCamera());
                        break;
                    case CameraType::THIRDPERSON:
                        inputHandler->SetCursorInvisibleAndMiddle(true);
                        m_thirdPersonCamera->Update(p_dt);
                        t_graphicModuleCameraManager.PushCameraToDevice(m_thirdPersonCamera->GetCamera());
                        break;
                    default:
                        break;
                }
            }
        }
        void CameraHandler::ChangeCamera(CameraType p_type) { m_currentCamera = p_type; }
        DirectX::XMFLOAT3 CameraHandler::GetActiveCameraPosition()
        {
            switch(m_currentCamera)
            {
                case CameraType::FREELOOK:
                    return m_freeLookCamera->GetCamera().GetCameraPosition();
                    break;
                case CameraType::THIRDPERSON:
                    return m_thirdPersonCamera->GetCamera().GetCameraPosition();
                    break;
                default:
                    break;
            }
        }
    }
}