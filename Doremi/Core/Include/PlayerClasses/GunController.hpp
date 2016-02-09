#pragma once
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}
namespace Doremi
{
    namespace Core
    {
        class Player;
        class GunController
        {
        public:
            GunController();
            virtual ~GunController();

            void StartFireGun(EntityID p_entityID, const DoremiEngine::Core::SharedContext& p_sharedContext);

            void StopFireGun(EntityID p_entityID, const DoremiEngine::Core::SharedContext& p_sharedContext);


            void Update(Player* p_player, const DoremiEngine::Core::SharedContext& p_sharedContext);
        };
    }
}