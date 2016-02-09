#pragma once


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

            void StartFireGun(Player* p_player, const DoremiEngine::Core::SharedContext& p_sharedContext);

            void StopFireGun(Player* p_player, const DoremiEngine::Core::SharedContext& p_sharedContext);


            void Update(Player* p_player, const DoremiEngine::Core::SharedContext& p_sharedContext);
        };
    }
}