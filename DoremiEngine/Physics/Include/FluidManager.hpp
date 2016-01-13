#include <vector>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        class FluidManager
        {
            public:
            virtual void CreateFluid(int p_id) = 0;
            virtual void CreateFluidParticles(int p_id, vector<XMFLOAT3>& p_positions, vector<XMFLOAT3>& p_velocities, vector<int>& p_indices) = 0;
            virtual vector<XMFLOAT3> GetParticlePositions(int p_id) = 0;
        };
    }
}