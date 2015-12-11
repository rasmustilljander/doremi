namespace DoremiEngine
{
    namespace Physics
    {
        class PhysicsMaterialManager
        {
            public:
            /**
            Creates a physical material of the specific attributes. Returns an ID to be used
            when interracting with the material.
            Just send 0.5,0.5,0.5 as parameters for basic results*/
            virtual int CreateMaterial(float p_staticFriction, float p_dynamicFriction, float p_restitution) = 0;
        };
    }
}