#pragma once
#include <DirectXMath.h>
#include <PhysX/PxPhysicsAPI.h>
#include <vector>

using namespace std;
using namespace DirectX;
using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        /**
        Class used to cook meshes into complex collision surfaces.*/
        class MeshCooker
        {
        public:
            MeshCooker(InternalPhysicsUtils& p_utils);
            virtual ~MeshCooker();

            // Cooks a set of vertex positions into a collision mesh
            PxTriangleMesh* CookMesh(vector<XMFLOAT3>& p_pertexPositions, vector<int>& p_indices);


        private:
            InternalPhysicsUtils& m_utils;
            PxCooking* m_cooker;
        };
    }
}