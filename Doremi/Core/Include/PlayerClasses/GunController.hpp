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

            void FireGun(int p_playerID, Player* p_player, const DoremiEngine::Core::SharedContext& p_sharedContext);
        };
    }
}