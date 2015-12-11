#include <CameraClasses/ThirdPersonCamera.hpp>
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
        }
    }
}