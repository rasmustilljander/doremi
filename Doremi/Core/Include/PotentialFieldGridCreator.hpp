#pragma once
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialField;
    }
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {

        class PotentialFieldGridCreator
        {
            public:
            PotentialFieldGridCreator(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~PotentialFieldGridCreator();
            void BuildGridUsingPhysicXAndGrid(DoremiEngine::AI::PotentialField* op_field);

            private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}