// Project specific
#include <Manager/LightManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/LightComponent.hpp>

// 3rd party
#include <DirectXMath.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {

        LightManager::LightManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "LightManager") {}

        LightManager::~LightManager() {}

        void LightManager::Update(double p_dt) {}

        void LightManager::OnEvent(Event* p_event) {}
    }
}