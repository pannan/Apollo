#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseMButtonUp : public EvtMouseMsg
	{
	public:
		EvtMouseMButtonUp(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseMButtonUp();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseMButtonUp> EvtMouseMButtonUpPtr;
}
