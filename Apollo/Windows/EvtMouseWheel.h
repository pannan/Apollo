#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseWheel : public EvtMouseMsg
	{
	public:
		EvtMouseWheel(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseWheel();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();

		int GetWheelDelta();
	};

	typedef std::shared_ptr<EvtMouseWheel> EvtMouseWheelPtr;
}
