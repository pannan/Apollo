#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseRButtonDown : public EvtMouseMsg
	{
	public:
		EvtMouseRButtonDown(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseRButtonDown();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseRButtonDown> EvtMouseRButtonDownPtr;
}
