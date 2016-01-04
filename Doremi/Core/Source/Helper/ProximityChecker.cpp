#pragma once
// Project specific
#include <Helper/ProximityChecker.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
// Utilities
#include <Utility/DebugLog/Include/ConsoleManager.hpp>

// Standard

namespace Doremi
{
    namespace Core
    {
        ProximityChecker* ProximityChecker::m_singleton = nullptr;
        ProximityChecker& ProximityChecker::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new ProximityChecker();
            }
            return *m_singleton;
        }

        ProximityChecker::ProximityChecker() {}
        ProximityChecker::~ProximityChecker() {}
        bool ProximityChecker::CheckProximityToEntity(size_t p_entityID, size_t p_entityToCheckID)
        {
            using namespace DirectX;
            int mask = (int)ComponentType::Transform;
            if(!EntityHandler::GetInstance().HasComponents(p_entityID, mask) || !EntityHandler::GetInstance().HasComponents(p_entityToCheckID, mask) ||
               !EntityHandler::GetInstance().HasComponents(p_entityID, (int)ComponentType::Range))
            {
                Utility::DebugLog::ConsoleManager& manager = Utility::DebugLog::ConsoleManager::GetInstance();

                // Default settings requires no parameters, will be named "standard"
                Utility::DebugLog::VirtualConsole& defaultConsole = manager.GetConsole();
                defaultConsole.LogText(Utility::DebugLog::LogTag::GAME, Utility::DebugLog::LogLevel::DEBUG_PRINT,
                                       "One of the entities provided did not have the needed component. ID1: %d ID2: %d", p_entityID, p_entityToCheckID);
                return false;
            }
            // Everything is good, the entities have the needed components
            RangeComponent* rangeComp = EntityHandler::GetInstance().GetComponentFromStorage<RangeComponent>(p_entityID);
            TransformComponent* firstTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityID);
            TransformComponent* secondTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_entityToCheckID);
            return IsInProximity(firstTransform->position, secondTransform->position, rangeComp->range);
        }
        std::vector<size_t> ProximityChecker::GetAllEntitysInProximity(size_t p_entityID)
        {
            std::vector<size_t> retVector;
            return retVector;
        }

        bool ProximityChecker::IsInProximity(const DirectX::XMFLOAT3& p_position1, const DirectX::XMFLOAT3& p_position2, const float& p_range)
        {
            using namespace DirectX;
            XMVECTOR vecBetweenEntities = XMLoadFloat3(&p_position2) - XMLoadFloat3(&p_position1);
            XMVECTOR distanceVec = XMVector3Length(vecBetweenEntities);
            float distance = *distanceVec.m128_f32;
            if(distance <= p_range)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
