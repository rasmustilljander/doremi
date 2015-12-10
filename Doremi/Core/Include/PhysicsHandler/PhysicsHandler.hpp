
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
            static void StartPhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            
            /**
            Intended as a test function for PhysX. Does alot of things.
            Should be removed. TODOJB*/
            void BigTestFunction();

            /**
            Updates the physics simulation once step*/
            void UpdatePhysics(float p_dt);

            private:
            PhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~PhysicsHandler();
            static PhysicsHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}