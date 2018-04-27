#pragma once

#include "IEvent.h"

namespace Apollo
{
	class EventManager;

	class IEventListener
	{
	public:
		IEventListener();
		virtual ~IEventListener();

		virtual std::wstring GetName() = 0;
		virtual bool HandleEvent(EventPtr pEvent) = 0;

		void RequestEvent(eEVENT e);
		void UnRequestEvent(eEVENT e);
		void SetEventManager(EventManager* pEventManager);

	private:
		EventManager*	m_pEventManager;
		std::vector< eEVENT > m_RequestedEvents;
	};
}
