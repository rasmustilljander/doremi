#include <Interface/PotentialField/PotentialField.hpp>
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldImpl : public PotentialField
        {
            public:
            PotentialFieldImpl();
            virtual ~PotentialFieldImpl();
            void SetGrid(const std::vector<std::vector<PotentialFieldGridPoint>>& p_grid) override;
            void AddActor(PotentialFieldActor* p_newActor) override;

            private:
            std::vector<std::vector<PotentialFieldGridPoint>> m_grid;
            std::vector<PotentialFieldActor*> m_actors;
        };
    }
}