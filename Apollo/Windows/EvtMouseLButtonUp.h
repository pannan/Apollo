#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseLButtonUp : public EvtMouseMsg
	{
	public:
		EvtMouseLButtonUp(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseLButtonUp();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseLButtonUp> EvtMouseLButtonUpPtr;
}
