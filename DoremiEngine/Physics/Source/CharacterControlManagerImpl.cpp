#include <Internal/CharacterControlManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
namespace DoremiEngine
{
    namespace Physics
    {
        CharacterControlManagerImpl::CharacterControlManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils)
        {
            m_manager = PxCreateControllerManager(*m_utils.m_worldScene);
        }
        CharacterControlManagerImpl::~CharacterControlManagerImpl() {}

        int CharacterControlManagerImpl::AddController(int p_id, XMFLOAT3 p_position, XMFLOAT2 p_dimensions)
        {
            // Set start attributes of the controller
            PxCapsuleControllerDesc desc;
            desc.position = PxExtendedVec3(p_position.x, p_position.y, p_position.z);
            desc.height = p_dimensions.x;
            desc.radius = p_dimensions.y;

            // Hard coded up vector
            desc.upDirection = PxVec3(0, 1, 0);

            m_controllers[p_id] = m_manager->createController(desc);

            // Redundant return?
            return p_id;
        }

        int CharacterControlManagerImpl::MoveController(int p_id, XMFLOAT3 p_discplacement, float p_dt)
        {
            /*
            Some variable that apperas to define when the controller stops moving
            Tweak as necessary. Possibly could be moved to some other place.*/
            float m_minDistTraveled = 0;
            // EMPTY FILTERS!
            PxControllerFilters filters;
            m_controllers[p_id]->move(PxVec3(p_discplacement.x, p_discplacement.y, p_discplacement.z), 0, p_dt, filters);

            // Redundant return?
            return p_id;
        }
    }
}