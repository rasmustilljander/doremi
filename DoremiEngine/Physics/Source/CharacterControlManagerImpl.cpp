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

        int CharacterControlManagerImpl::AddController(int p_id, int p_matID, XMFLOAT3 p_position, XMFLOAT2 p_dimensions)
        {
            // Set start attributes of the controller
            PxCapsuleControllerDesc desc;
            desc.setToDefault();
            desc.position = PxExtendedVec3(p_position.x, p_position.y, p_position.z);
            desc.height = p_dimensions.x;
            desc.radius = p_dimensions.y;
            desc.material = m_utils.m_physicsMaterialManager->GetMaterial(p_matID); // DANGER! Assumes it already has material
            desc.stepOffset = 0.1;
            // Hard coded up vector
            desc.upDirection = PxVec3(0, 1, 0);

            bool derp = desc.isValid();

            PxController* controller = m_manager->createController(desc);

            m_controllers[p_id] = m_manager->createController(desc);

            SetCallback(p_id, (1 << 0), (1 << 0));

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

        XMFLOAT3 CharacterControlManagerImpl::GetPosition(int p_id)
        {
            PxExtendedVec3 p = m_controllers[p_id]->getPosition();
            return XMFLOAT3(p.x, p.y, p.z);
        }

        XMFLOAT4 CharacterControlManagerImpl::GetOrientation(int p_id)
        {
            PxQuat q = m_controllers[p_id]->getActor()->getGlobalPose().q;
            return XMFLOAT4(q.x, q.y, q.z, q.w);
        }

        void CharacterControlManagerImpl::SetCallback(int p_bodyID, int p_filterGroup, int p_filterMask)
        {
            PxFilterData filterData;
            filterData.word0 = p_filterGroup; // Own ID
            filterData.word1 = p_filterMask; // ID mask to filter pairs that trigger contact callback
            PxRigidActor* actor = m_controllers[p_bodyID]->getActor();
            int numShapes = actor->getNbShapes();
            // Magic allocation of memory (i think)
            PxShape** shapes = (PxShape**)m_utils.m_allocator.allocate(sizeof(PxShape*) * numShapes, 0, __FILE__, __LINE__);
            actor->getShapes(shapes, numShapes);
            for(size_t i = 0; i < numShapes; i++)
            {
                PxShape* shape = shapes[i];
                shape->setSimulationFilterData(filterData);
            }
            if(shapes)
            {
                m_utils.m_allocator.deallocate(shapes);
                shapes = NULL;
            }
        }
    }
}