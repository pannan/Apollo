#pragma once

#include "IEvent.h"

namespace Apollo
{
	class EvtWindowMsg : public IEvent
	{
	public:
		EvtWindowMsg(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtWindowMsg();

		HWND GetWindowHandle();
		unsigned int GetWParam();
		unsigned int GetLParam();

	protected:
		HWND			m_hWnd;
		unsigned int	m_wparam;
		unsigned int	m_lparam;
	};

	typedef std::shared_ptr<EvtWindowMsg> EvtWindowMsgPtr;
}
