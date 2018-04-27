#pragma once

#include "IEvent.h"
#include "IEventListener.h"
#include "Singleton.h"

namespace Apollo
{
	class EventManager : public SingletonEx<EventManager>
	{
	public:
		EventManager();
		virtual ~EventManager();

		bool AddEventListener(eEVENT EventID, IEventListener* pListener);
		bool DelEventListener(eEVENT EventID, IEventListener* pListener);

		bool ProcessEvent(EventPtr pEvent);
		bool QueueEvent(EventPtr pEvent);
		bool ProcessEventQueue();

		//static EventManager* Get();

	protected:
		std::vector< IEventListener* > m_EventHandlers[NUM_EVENTS];
		std::vector< EventPtr > m_EventQueue;

	//	static EventManager* m_spEventManager;
	};
}
