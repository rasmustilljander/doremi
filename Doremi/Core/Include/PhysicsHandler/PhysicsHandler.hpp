
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
        /**
        Handles physics tasks such as creating various physics objects
        Changes are this handler ends up being entierly useless*/
        class PhysicsHandler
        {
            public:
            /**
            Gets the instance of the PhysicsHandler
            WARNING!! StartPhysicsHandler must have been called first*/
            static PhysicsHandler* GetInstance();
            /**
            Creates the singleton and ensures it has access to the engine*/
            void StartPhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            private:
            PhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~PhysicsHandler();
            static PhysicsHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}