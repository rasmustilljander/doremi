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
        class GunController
        {
        public:
            GunController();
            virtual ~GunController();

            void FireGun(int p_playerID, const DoremiEngine::Core::SharedContext& p_sharedContext);
        };
    }
}