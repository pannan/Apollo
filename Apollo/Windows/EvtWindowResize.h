#pragma once

#include "EvtWindowMsg.h"

namespace Apollo
{
	class EvtWindowResize : public EvtWindowMsg
	{
	public:
		EvtWindowResize(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtWindowResize();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();

		int NewWidth();
		int NewHeight();
		bool IsMaxHide();	// Message is sent to all pop-up windows when some other window is maximized.
		bool IsMaximized();	// The window has been maximized.
		bool IsMaxShow();	// Message is sent to all pop-up windows when some other window has been restored to its former size.
		bool IsMinimized(); // The window has been minimized.
		bool IsRestored();	// The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.
	};

	typedef std::shared_ptr<EvtWindowResize> EvtWindowResizePtr;
}
