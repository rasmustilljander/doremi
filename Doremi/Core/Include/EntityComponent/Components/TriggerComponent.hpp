#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        enum class TriggerType : int
        {
            NoTrigger = 0x00,
            GoalTrigger = 0x01,
            NewSpawnPointTrigger = 0x02
        };
        struct TriggerComponent
        {
            DirectX::XMFLOAT3 dimensions;
            TriggerType triggerType;
            TriggerComponent(DirectX::XMFLOAT3 p_dimensions, TriggerType p_triggerType) : dimensions(p_dimensions), triggerType(p_triggerType) {}
            TriggerComponent() : dimensions(DirectX::XMFLOAT3(0, 0, 0)), triggerType(TriggerType::NoTrigger) {}
        };
    }
}