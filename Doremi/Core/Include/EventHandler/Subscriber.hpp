#pragma once
#include <EventHandler/Events/Event.hpp>

namespace Doremi
{
	namespace Core
	{
		/** Every class that want to be able to recieve events needs to inherit Subscriber*/
		class Subscriber
		{
		public:
			virtual void OnEvent(Event* p_event) = 0;

		};
	}
}