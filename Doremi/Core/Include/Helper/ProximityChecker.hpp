#pragma once
// Project specific

// Standard
#include <vector>
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
        Uses a range component to check if entities are in proximity of eachother
        */
        class ProximityChecker
        {
            public:
            static ProximityChecker& GetInstance();
            virtual ~ProximityChecker();
            /**
            Returns true if the entity to check is in the proximity of entity
            */
            bool CheckProximityToEntity(size_t p_entityID, size_t p_entityToCheckID);
            /**
            Returns a list of all entitys that are in proximity
            */
            std::vector<size_t> GetAllEntitysInProximity(size_t p_entityID);

            protected:
            static ProximityChecker* m_singleton;
            ProximityChecker();

            // Help functions
            bool IsInProximity(const DirectX::XMFLOAT3& p_position1, const DirectX::XMFLOAT3& p_position2, const float& p_range);
        };
    }
}
