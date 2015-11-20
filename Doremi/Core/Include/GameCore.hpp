#pragma once
class EntityInterface;
namespace Doremi
{
    namespace Core
    {
        class GameCore
        {
            public:
            GameCore();
            virtual ~GameCore();
            void Initialize();
            void StartCore();

            private:
            EntityInterface* m_enitiyInterface;
        };
    }
}