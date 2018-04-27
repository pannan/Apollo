#pragma once

#include "EvtKeyboardMsg.h"

namespace Apollo
{
	class EvtKeyUp : public EvtKeyboardMsg
	{
	public:
		EvtKeyUp(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtKeyUp();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtKeyUp> EvtKeyUpPtr;
}
