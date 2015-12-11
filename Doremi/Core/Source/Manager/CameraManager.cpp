#include <Manager/CameraManager.hpp>
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
        CameraManager::CameraManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext), m_graphicModuleCameraManager(m_sharedContext.GetGraphicModule().GetSubModuleManager().GetCameraManager())
        {
            using namespace DirectX;
            XMFLOAT4X4 projection;
            XMMATRIX mat = XMMatrixTranspose(XMMatrixPerspectiveFovLH(90 * 3.14 / 180.0f, 800.0f / 600.0f, 0.1f, 1000.0f));
            XMStoreFloat4x4(&projection, mat);
            // TODOKO Rename cameramanager on graphic module
            m_thirdPersonCamera = m_graphicModuleCameraManager.BuildNewCamera(projection);
            m_freeLookCamera = m_graphicModuleCameraManager.BuildNewCamera(projection);
            m_currentCamera = CameraType::FREELOOK;
        }

        CameraManager::~CameraManager() {}

        void CameraManager::Update(double p_dt)
        {
            switch(m_currentCamera)
            {
                case CameraType::FREELOOK:
                    m_graphicModuleCameraManager.PushCameraToDevice(*m_freeLookCamera);
                    break;
                case CameraType::THIRDPERSON:
                    m_graphicModuleCameraManager.PushCameraToDevice(*m_thirdPersonCamera);
                    break;
                default:
                    break;
            }
        }

        void CameraManager::OnEvent(Event* p_event) {}
    }
}