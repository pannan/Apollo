#pragma once

#include "EvtMouseMsg.h"
namespace Apollo
{
	class EvtMouseMove : public EvtMouseMsg
	{
	public:
		EvtMouseMove(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseMove();

		virtual std::wstring GetEventName();
		virtual eEVENT GetEventType();
	};

	typedef std::shared_ptr<EvtMouseMove> EvtMouseMovePtr;
}
