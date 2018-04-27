#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseRButtonUp : public EvtMouseMsg
	{
	public:
		EvtMouseRButtonUp(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseRButtonUp();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseRButtonUp> EvtMouseRButtonUpPtr;
}
