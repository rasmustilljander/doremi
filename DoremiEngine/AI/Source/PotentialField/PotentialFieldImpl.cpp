#include <Internal/PotentialField/PotentialFieldImpl.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        PotentialFieldImpl::PotentialFieldImpl() {}
        PotentialFieldImpl::~PotentialFieldImpl() {}
        void PotentialFieldImpl::SetGrid(const std::vector<std::vector<PotentialFieldGridPoint>>& p_grid)
        {
            // m_grid = p_grid;
            m_grid = std::move(p_grid); // TODOKO Might not work...
        }
        void PotentialFieldImpl::AddActor(PotentialFieldActor* p_newActor) { m_actors.push_back(p_newActor); }
    }
}