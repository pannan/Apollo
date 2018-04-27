#pragma once

#include "EvtWindowMsg.h"

namespace Apollo
{
	class EvtMouseMsg : public EvtWindowMsg
	{
	public:
		EvtMouseMsg(HWND hwnd, unsigned int wparam, long lparam);
		virtual ~EvtMouseMsg();

		bool ControlKeyDown();
		bool LButtonDown();
		bool MButtonDown();
		bool RButtonDown();
		bool ShiftKeyDown();

		int GetX();
		int GetY();
	};

	typedef std::shared_ptr<EvtMouseMsg> EvtMouseMsgPtr;
}
