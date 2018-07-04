#include "stdafx.h"
#include "EventManager.h"
#include "LogManager.h"
using namespace Apollo;

void EventManager::addMouseEventListener(IEventListener* listener)
{
	if (listener == nullptr)
	{
		LogManager::getInstance().log("MouseEventListener add listener is null!");
		return;
	}

	for each (IEventListener* var in m_mouseEventListenerList)
	{
		if (var == listener)
		{
			LogManager::getInstance().log("MouseEventListener already exit!");
			return;
		}
	}

	m_mouseEventListenerList.push_back(listener);
}

void EventManager::removeMouseEventListener(IEventListener* listener)
{
	for (std::list<IEventListener*>::iterator it = m_mouseEventListenerList.begin(); it != m_mouseEventListenerList.end();++it)
	{
		if (*it == listener)
		{
			m_mouseEventListenerList.erase(it);
			return;
		}
	}
}

void EventManager::notifyMouseMoveEvent(MouseEventArg* arg)
{
	for each (IEventListener* var in m_mouseEventListenerList)
	{
		var->onMouseMoveEvent(arg);
	}
}