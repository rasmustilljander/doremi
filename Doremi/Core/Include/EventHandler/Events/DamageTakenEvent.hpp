#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
namespace Doremi
{
    namespace Core
    {
        struct DamageTakenEvent : public Event
        {
            float damage;
            size_t entityId;
            DamageTakenEvent(const float& p_damage, const size_t& p_entityId) : damage(p_damage), entityId(p_entityId), Event(EventType::DamageTaken)
            {
            }
            DamageTakenEvent() : damage(0), Event(EventType::DamageTaken) {}
        };
    }
}