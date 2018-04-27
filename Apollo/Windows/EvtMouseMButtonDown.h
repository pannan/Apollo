#pragma once

#include "EvtMouseMsg.h"

namespace Apollo
{
	class EvtMouseMButtonDown : public EvtMouseMsg
	{
	public:
		EvtMouseMButtonDown(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseMButtonDown();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseMButtonDown> EvtMouseMButtonDownPtr;
}