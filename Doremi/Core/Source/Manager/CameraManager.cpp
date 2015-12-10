#include <Manager/CameraManager.h>

// Third party
#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        CameraManager::CameraManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext) {}


        CameraManager::~CameraManager() {}

        void CameraManager::Update(double p_dt) {}

        void CameraManager::OnEvent(Event* p_event) {}
    }
}