#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseLButtonDown : public EvtMouseMsg
	{
	public:
		EvtMouseLButtonDown(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseLButtonDown();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseLButtonDown> EvtMouseLButtonDownPtr;
}
