#pragma once

#include "Singleton.h"
#include "IEventListener.h"


namespace Apollo
{
	class EventManager : public SingletonEx<EventManager>
	{
	public:

		void		addMouseEventListener(IEventListener* listener);

		void		addKeyDownEventListener(IEventListener* listener);

		void		removeMouseEventListener(IEventListener* listener);

		void		removeKeyDownEventListener(IEventListener* listener);

		void		notifyMouseMoveEvent(MouseEventArg* arg);

		void		notifyKeyDownEvent(KeyCode code);

	protected:

		std::list<IEventListener*>		m_mouseEventListenerList;
		std::list<IEventListener*>		m_keydownEventListenerList;

	private:
	};
}
