#pragma once

#include "Singleton.h"
#include "IEventListener.h"

namespace Apollo
{
	class EventManager : public SingletonEx<EventManager>
	{
	public:

		void		addMouseEventListener(IEventListener* listener);

		void		removeMouseEventListener(IEventListener* listener);

		void		notifyMouseMoveEvent(MouseEventArg* arg);

	protected:

		std::list<IEventListener*>		m_mouseEventListenerList;

	private:
	};
}
