#pragma once

#include "EvtKeyboardMsg.h"

namespace Apollo
{
	class EvtKeyDown : public EvtKeyboardMsg
	{
	public:
		EvtKeyDown(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtKeyDown();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtKeyDown> EvtKeyDownPtr;
}
