#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ExampleEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/SpecialEntityCreatedEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/TriggerEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
#include <Doremi/Core/Include/EventHandler/Events/DamageTakenEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerRespawnEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/GunFireToggleEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlaySoundEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/SetHealthEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/SetTransformEvent.hpp>

#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>


namespace Doremi
{
    namespace Core
    {
        /**
            Interpets event from read stream and returns it
        */
        static Event* InterpetEvent(NetworkStreamer& p_streamer, uint32_t& op_BitssRead)
        {
            // Hard coded type
            EventType p_eventType = (EventType)p_streamer.ReadUnsignedInt8();
            op_BitssRead += sizeof(uint8_t) * 8;

            Event* p_newEvent;

            // Create event based on type
            switch(p_eventType)
            {
                case Doremi::Core::EventType::Undefined:
                    p_newEvent = new ExampleEvent();

                    break;
                case Doremi::Core::EventType::EntityCreated:
                    p_newEvent = new EntityCreatedEvent();

                    break;
                case Doremi::Core::EventType::RemoveEntity:
                    p_newEvent = new RemoveEntityEvent();

                    break;
                case Doremi::Core::EventType::Example:
                    p_newEvent = new ExampleEvent();

                    break;
                case Doremi::Core::EventType::PlayerCreation:
                    p_newEvent = new PlayerCreationEvent();

                    break;
                case Doremi::Core::EventType::RangedEnemyCreated:
                    p_newEvent = new SpecialEntityCreatedEvent();

                    break;
                case Doremi::Core::EventType::MeleeEnemyCreated:
                    p_newEvent = new SpecialEntityCreatedEvent();

                    break;
                case Doremi::Core::EventType::Trigger:
                    p_newEvent = new TriggerEvent();

                    break;
                case Doremi::Core::EventType::ChangeMenuState:
                    p_newEvent = new ChangeMenuState();

                    break;
                case Doremi::Core::EventType::DamageTaken:
                    p_newEvent = new DamageTakenEvent();

                    break;
                case Doremi::Core::EventType::PlayerRespawn:
                    p_newEvent = new PlayerRespawnEvent();
                    break;

                case Doremi::Core::EventType::GunFireToggle:
                    p_newEvent = new GunFireToggleEvent();
                    break;

                case Doremi::Core::EventType::PlaySound:
                    p_newEvent = new PlaySoundEvent();
                    break;

                case Doremi::Core::EventType::SetHealth:
                    p_newEvent = new SetHealthEvent();
                    break;

                case Doremi::Core::EventType::SetTransform:
                    p_newEvent = new SetTransformEvent();
                    break;

                default:
                    p_newEvent = new ExampleEvent();
                    break;
            }

            // Set type
            p_newEvent->eventType = p_eventType;

            // Read based on type
            p_newEvent->Read(&p_streamer, op_BitssRead);

            return p_newEvent;
        }
    }
}