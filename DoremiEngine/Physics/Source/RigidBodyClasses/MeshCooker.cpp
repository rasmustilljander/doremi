// This class
#include <Internal/RigidBodyClasses/MeshCooker.hpp>

#include <Internal/PhysicsModuleImplementation.hpp>

namespace DoremiEngine
{
    namespace Physics
    {
        MeshCooker::MeshCooker(InternalPhysicsUtils& p_utils) : m_utils(p_utils)
        {
            m_cooker = PxCreateCooking(PX_PHYSICS_VERSION, *m_utils.m_foundation, PxCookingParams(m_utils.m_physics->getTolerancesScale()));
            if(!m_cooker)
            {
                // shit happened
                throw runtime_error("initializing mesh cooker failed");
            }
        }
        MeshCooker::~MeshCooker() {}

        PxTriangleMesh* MeshCooker::CookMesh(vector<XMFLOAT3>& p_vertexPositions, vector<int>& p_indices)
        {
            // Cast vectors into pointers
            PxVec3* positionsPX = reinterpret_cast<PxVec3*>(&p_vertexPositions[0]);
            PxU32* indicesPX = reinterpret_cast<PxU32*>(&p_indices[0]);

            // Create the mesh data description
            PxTriangleMeshDesc meshDesc;
            meshDesc.setToDefault();
            meshDesc.points.count = p_vertexPositions.size();
            meshDesc.points.stride = sizeof(PxVec3);
            meshDesc.points.data = positionsPX;

            meshDesc.triangles.count = p_indices.size() / 3;
            meshDesc.triangles.stride = sizeof(PxU32) * 3;
            meshDesc.triangles.data = indicesPX;
            meshDesc.flags = PxMeshFlags(0);

            // Cook the mesh
            PxDefaultMemoryOutputStream writeBuffer;
            bool status = m_cooker->cookTriangleMesh(meshDesc, writeBuffer);
            if(!status)
            {
                return NULL;
            }

            // Get the data and return
            PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
            return m_utils.m_physics->createTriangleMesh(readBuffer);
        }
    }
}